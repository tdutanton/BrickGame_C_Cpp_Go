package main

/*
#cgo CFLAGS: -I.
#include <stdlib.h>
#include "s21_backend_race.h"
*/
import "C"

import (
	"fmt"
	"math/rand"
	"os"
	"strconv"
	"time"
	"unsafe"
)

const (
	SPACEBAR     = C.int(C.SPACEBAR)
	EMPTY_BLOCK  = C.int(C.EMPTY_BLOCK)
	PLAYER_BLOCK = C.int(C.CURRENT_FIGURE_BLOCK)
	ENEMY_BLOCK  = C.int(C.ATTACHED_BLOCK)
	ROWS         = C.int(C.ROWS)
	COLS         = C.int(C.COLS)
	LEVEL_STEP   = C.int(C.LEVEL_STEP)
	MAX_FIG_ROWS = C.int(C.MAX_FIG_ROWS)
	MAX_FIG_COLS = C.int(C.MAX_FIG_COLS)
	START_SPEED  = C.int(C.START_SPEED)
)

const (
	Pause     = C.Pause
	Start     = C.Start
	Terminate = C.Terminate
	Left      = C.Left
	Right     = C.Right
	Up        = C.Up
	Down      = C.Down
	Action    = C.Action
)

const (
	START_STATE     = C.START_STATE
	SPAWN_STATE     = C.SPAWN_STATE
	MOVING_STATE    = C.MOVING_STATE
	SHIFTING_STATE  = C.SHIFTING_STATE
	ATTACHING_STATE = C.ATTACHING_STATE
	GAME_OVER_STATE = C.GAME_OVER_STATE
	PAUSE_STATE     = C.PAUSE_STATE
	TERMINATE_STATE = C.TERMINATE_STATE
)

const (
	CAR_HEIGHT = 4
	CAR_WIDTH  = 3
)

var playerCar = [CAR_HEIGHT][CAR_WIDTH]C.int{
	{EMPTY_BLOCK, PLAYER_BLOCK, EMPTY_BLOCK},
	{PLAYER_BLOCK, PLAYER_BLOCK, PLAYER_BLOCK},
	{EMPTY_BLOCK, PLAYER_BLOCK, EMPTY_BLOCK},
	{PLAYER_BLOCK, PLAYER_BLOCK, PLAYER_BLOCK},
}

var enemyCar = [CAR_HEIGHT][CAR_WIDTH]C.int{
	{EMPTY_BLOCK, ENEMY_BLOCK, EMPTY_BLOCK},
	{ENEMY_BLOCK, ENEMY_BLOCK, ENEMY_BLOCK},
	{EMPTY_BLOCK, ENEMY_BLOCK, EMPTY_BLOCK},
	{ENEMY_BLOCK, ENEMY_BLOCK, ENEMY_BLOCK},
}

type Car struct {
	X, Y int
}

// RaceGameInfo - like FullInfo with GameInfo_t and other stuff
type RaceGameInfo struct {
	// Info - GameInfo_t struct - legacy from other BrickGames
	Info C.GameInfo_t
	// Player - Player's car struct
	Player Car
	// State - current game state
	State C.game_state
	// CurrentAction - current userPush
	CurrentAction C.UserAction_t
	// Timer - last enemies' step time
	Timer int64
	// SpawnTimer - last enemie's spawn time
	SpawnTimer int64
	// EnemySpeed - base enemie's speed
	EnemySpeed time.Duration
	// Holding - is key is holding
	Holding bool
}

func updateTime() int64 {
	return time.Now().UnixMilli()
}

func boolToCInt(b bool) C.int {
	if b {
		return 1
	}
	return 0
}

var (
	// RaceGame - global Game (main stuff for the Game)
	RaceGame    *RaceGameInfo
	fullInfoPtr *C.full_game_info_t
	FieldArea   [][]*C.int
	NextArea    [][]*C.int
	enemies     []Car
)

const (
	PlayerStartX = int(COLS / 2)
	PlayerStartY = ROWS - CAR_WIDTH - 1
)

func (c *Car) StepDown() {
	c.Y++
}

func (c *Car) StepUp() {
	c.Y--
}

func (c *Car) StepLeft() {
	c.X--
}

func (c *Car) StepRight() {
	c.X++
}

func (c *Car) SetPlayerOnStart() {
	c.X = int(PlayerStartX)
	c.Y = int(PlayerStartY)
}

func InitNewGame() *RaceGameInfo {
	defer func() {
		if r := recover(); r != nil {
			DestroyGame()
			panic(r)
		}
	}()

	DestroyGame()
	enemies = nil
	RaceGame = &RaceGameInfo{}

	var game C.GameInfo_t

	game.field = createEmptyArea(int(ROWS), int(COLS))
	game.next = createEmptyArea(int(MAX_FIG_ROWS), int(MAX_FIG_COLS))

	game.score = 0
	game.level = 1
	game.high_score = C.int(loadHighScore())
	game.speed = C.START_SPEED
	game.pause = 0
	RaceGame.EnemySpeed = time.Duration(START_SPEED) * time.Millisecond

	FieldArea = make([][]*C.int, ROWS)
	for i := 0; i < int(ROWS); i++ {
		FieldArea[i] = make([]*C.int, COLS)
	}

	NextArea = make([][]*C.int, MAX_FIG_ROWS)
	for i := 0; i < int(MAX_FIG_ROWS); i++ {
		NextArea[i] = make([]*C.int, MAX_FIG_COLS)
	}

	convertFieldToSlice(game.field, FieldArea, int(ROWS), int(COLS))
	convertFieldToSlice(game.next, NextArea, int(MAX_FIG_ROWS), int(MAX_FIG_COLS))
	setEnemyCarOnNext()

	RaceGame.Info = game
	RaceGame.Player.SetPlayerOnStart()
	RaceGame.State = C.START_STATE
	RaceGame.CurrentAction = C.Start
	RaceGame.Holding = false
	RaceGame.Timer = updateTime()
	RaceGame.SpawnTimer = updateTime()

	setPlayerOnArea(int(RaceGame.Player.Y), int(RaceGame.Player.X))
	update_ptr_full()
	return RaceGame
}

//export update_ptr_full
func update_ptr_full() *C.full_game_info_t {
	full := UpdateFullInfo()

	if fullInfoPtr == nil {
		fullInfoPtr = (*C.full_game_info_t)(C.malloc(C.size_t(unsafe.Sizeof(C.full_game_info_t{}))))
		if fullInfoPtr == nil {
			return nil
		}
	}

	fullInfoPtr.info = full.Info
	fullInfoPtr.Player.X = C.int(full.Player.X)
	fullInfoPtr.Player.Y = C.int(full.Player.Y)
	fullInfoPtr.state = full.State
	fullInfoPtr.current_action = full.CurrentAction
	fullInfoPtr.holding_ = boolToCInt(full.Holding)

	return fullInfoPtr
}

func DestroyGame() {
	if RaceGame == nil {
		return
	}

	if RaceGame.Info.field != nil {
		rows := (*[1 << 30]*C.int)(unsafe.Pointer(RaceGame.Info.field))[:int(ROWS):int(ROWS)]
		for i := 0; i < int(ROWS); i++ {
			if rows[i] != nil {
				C.free(unsafe.Pointer(rows[i]))
			}
		}
		C.free(unsafe.Pointer(RaceGame.Info.field))
		RaceGame.Info.field = nil
	}

	if RaceGame.Info.next != nil {
		rows := (*[1 << 30]*C.int)(unsafe.Pointer(RaceGame.Info.next))[:int(MAX_FIG_ROWS):int(MAX_FIG_ROWS)]
		for i := 0; i < int(MAX_FIG_ROWS); i++ {
			if rows[i] != nil {
				C.free(unsafe.Pointer(rows[i]))
			}
		}
		C.free(unsafe.Pointer(RaceGame.Info.next))
		RaceGame.Info.next = nil
	}

	if fullInfoPtr != nil {
		C.free(unsafe.Pointer(fullInfoPtr))
		fullInfoPtr = nil
	}

	FieldArea = nil
	NextArea = nil
	enemies = nil
	RaceGame = nil
}

func setPlayerOnArea(y, x int) {
	if FieldArea == nil || RaceGame == nil || y < 0 || x < 0 {
		return
	}

	for i := 0; i < CAR_HEIGHT; i++ {
		yi := y + i
		if yi >= len(FieldArea) || FieldArea[yi] == nil {
			continue
		}
		for j := 0; j < CAR_WIDTH; j++ {
			xj := x + j
			if xj >= len(FieldArea[yi]) || FieldArea[yi][xj] == nil {
				continue
			}
			*FieldArea[yi][xj] = playerCar[i][j]
		}
	}
}

func convertFieldToSlice(field **C.int, slice [][]*C.int, height, width int) {
	if field == nil {
		return
	}

	rows := (*[1 << 30]*C.int)(unsafe.Pointer(field))[:height:height]
	for i := 0; i < height; i++ {
		if rows[i] == nil {
			continue
		}
		row := (*[1 << 30]C.int)(unsafe.Pointer(rows[i]))[:width:width]
		for j := 0; j < width; j++ {
			slice[i][j] = &row[j]
		}
	}
}

func createEmptyArea(height, width int) **C.int {
	area := (**C.int)(C.calloc(C.size_t(height), C.size_t(unsafe.Sizeof((*C.int)(nil)))))
	if area == nil {
		panic("Failed to allocate memory for area")
	}

	rows := (*[1 << 30]*C.int)(unsafe.Pointer(area))[:height:height]
	for i := 0; i < height; i++ {
		rows[i] = (*C.int)(C.calloc(C.size_t(width), C.size_t(unsafe.Sizeof(C.int(0)))))
		if rows[i] == nil {
			for j := 0; j < i; j++ {
				C.free(unsafe.Pointer(rows[j]))
			}
			C.free(unsafe.Pointer(area))
			panic(fmt.Sprintf("Failed to allocate memory for row %d", i))
		}

		row := (*[1 << 30]C.int)(unsafe.Pointer(rows[i]))[:width:width]
		for j := 0; j < width; j++ {
			row[j] = EMPTY_BLOCK
		}
	}
	return area
}

func UpdateFullInfo() *RaceGameInfo {
	if RaceGame == nil {
		RaceGame = &RaceGameInfo{}
	}

	if RaceGame.Info.field == nil {
		RaceGame.Info.field = createEmptyArea(int(ROWS), int(COLS))
		FieldArea = make([][]*C.int, ROWS)
		for i := 0; i < int(ROWS); i++ {
			FieldArea[i] = make([]*C.int, COLS)
		}
		convertFieldToSlice(RaceGame.Info.field, FieldArea, int(ROWS), int(COLS))
	}

	if RaceGame.Info.next == nil {
		RaceGame.Info.next = createEmptyArea(int(MAX_FIG_ROWS), int(MAX_FIG_COLS))
		NextArea = make([][]*C.int, MAX_FIG_ROWS)
		for i := 0; i < int(MAX_FIG_ROWS); i++ {
			NextArea[i] = make([]*C.int, MAX_FIG_COLS)
		}
		convertFieldToSlice(RaceGame.Info.next, NextArea, int(MAX_FIG_ROWS), int(MAX_FIG_COLS))
	}

	return RaceGame
}

//export updateCurrentState
func updateCurrentState() C.GameInfo_t {
	fullInfo := UpdateFullInfo()
	fsm()
	update_ptr_full()
	return fullInfo.Info
}

//export userInput
func userInput(action C.UserAction_t, hold bool) {
	fullInfo := UpdateFullInfo()
	if fullInfo != nil {
		fullInfo.CurrentAction = action
		if action == C.Up {
			fullInfo.Holding = hold
		}
	}
}

func canSpawnAt(x, y int) bool {
	newEnemy := Car{X: x, Y: y}
	for _, e := range enemies {
		if checkCarsCollision(newEnemy, e) {
			return false
		}
	}
	return true
}

func checkCarsCollision(a, b Car) bool {
	return a.X < b.X+CAR_WIDTH && a.X+CAR_WIDTH > b.X &&
		a.Y < b.Y+CAR_HEIGHT && a.Y+CAR_HEIGHT > b.Y
}

func isPlayerPathBlocked() bool {
	playerY := RaceGame.Player.Y
	for x := 0; x <= int(COLS)-CAR_WIDTH; x++ {
		possiblePlayerPos := Car{X: x, Y: playerY}
		blocked := false
		for _, e := range enemies {
			if checkCarsCollision(possiblePlayerPos, e) {
				blocked = true
				break
			}
		}
		if !blocked {
			return false
		}
	}
	return true
}

func spawnEnemy() {
	maxAttempts := 10
	for attempt := 0; attempt < maxAttempts; attempt++ {
		newEnemy := Car{
			X: rand.Intn(int(COLS - CAR_WIDTH)),
			Y: 0,
		}
		if !canSpawnAt(newEnemy.X, newEnemy.Y) {
			continue
		}
		tempEnemies := append(enemies, newEnemy)
		if hasSafePath(RaceGame.Player, tempEnemies) {
			enemies = tempEnemies
			return
		}
	}
}

func hasSafePath(player Car, enemies []Car) bool {
	visited := make(map[[2]int]bool)
	queue := []Car{player}

	for len(queue) > 0 {
		cur := queue[0]
		queue = queue[1:]
		if cur.Y <= 0 {
			return true
		}

		dirs := []int{-1, 0, 1}
		for _, dx := range dirs {
			next := Car{X: cur.X + dx, Y: cur.Y - 1}

			if next.X < 0 || next.X > int(COLS-CAR_WIDTH) {
				continue
			}

			pos := [2]int{next.X, next.Y}
			if visited[pos] {
				continue
			}

			if !collides(next, enemies) {
				visited[pos] = true
				queue = append(queue, next)
			}
		}
	}
	return false
}

func collides(c Car, enemies []Car) bool {
	for _, e := range enemies {
		if checkCarsCollision(c, e) {
			return true
		}
	}
	return false
}

func (g *RaceGameInfo) moveEnemies() {
	alive := enemies[:0]
	for i := range enemies {
		enemies[i].Y++
		if enemies[i].Y < int(ROWS) {
			alive = append(alive, enemies[i])
		} else {
			g.Info.score++
			g.updateLevel()
		}
	}
	enemies = alive
}

func (g *RaceGameInfo) updateLevel() {
	if g.Info.score%5 == 0 && g.Info.level < 10 {
		g.Info.level++
		g.EnemySpeed -= 50 * time.Millisecond
		if g.EnemySpeed < 200*time.Millisecond {
			g.EnemySpeed = 200 * time.Millisecond
		}
		g.Info.speed = C.int(g.EnemySpeed / time.Millisecond)
	}
}

func checkCollision() bool {
	if RaceGame == nil {
		return false
	}

	player := RaceGame.Player

	for _, e := range enemies {
		if player.X < e.X+CAR_WIDTH && player.X+CAR_WIDTH > e.X &&
			player.Y < e.Y+CAR_HEIGHT && player.Y+CAR_HEIGHT > e.Y {
			return true
		}
	}
	return false
}

func clearField() {
	if FieldArea == nil {
		return
	}

	for i := 0; i < len(FieldArea); i++ {
		if FieldArea[i] == nil {
			continue
		}
		for j := 0; j < len(FieldArea[i]); j++ {
			if FieldArea[i][j] != nil {
				*FieldArea[i][j] = EMPTY_BLOCK
			}
		}
	}
}

func drawObjects() {
	if RaceGame == nil || RaceGame.State == C.GAME_OVER_STATE {
		return
	}
	clearField()
	setPlayerOnArea(int(RaceGame.Player.Y), int(RaceGame.Player.X))
	for _, e := range enemies {
		for i := 0; i < CAR_HEIGHT; i++ {
			for j := 0; j < CAR_WIDTH; j++ {
				x := e.X + j
				y := e.Y + i
				if y >= 0 && y < int(ROWS) && x >= 0 && x < int(COLS) {
					if FieldArea[y][x] != nil {
						*FieldArea[y][x] = enemyCar[i][j]
					}
				}
			}
		}
	}
}

func (g *RaceGameInfo) handleMovement() {
	switch g.CurrentAction {
	case C.Left:
		if int(g.Player.X) > 0 {
			g.Player.StepLeft()
		}
	case C.Right:
		if int(g.Player.X) < int(COLS)-CAR_WIDTH {
			g.Player.StepRight()
		}
	}
	g.CurrentAction = C.Start
}

func fsm() {
	switch RaceGame.State {
	case C.START_STATE:
		if RaceGame.CurrentAction == C.Start {
			InitNewGame()
			RaceGame.State = C.SPAWN_STATE
		}
	case C.SPAWN_STATE:
		spawnEnemy()
		RaceGame.Timer = updateTime()
		RaceGame.SpawnTimer = updateTime()
		RaceGame.State = C.MOVING_STATE
	case C.MOVING_STATE:
		if RaceGame.CurrentAction == C.Pause {
			RaceGame.togglePause()
			RaceGame.CurrentAction = C.Start
			break
		}
		RaceGame.handleMovement()
		tick()
		drawObjects()
		if checkCollision() {
			RaceGame.State = C.GAME_OVER_STATE
			RaceGame.CurrentAction = C.Pause
			break
		}
	case C.PAUSE_STATE:
		if RaceGame.CurrentAction == C.Pause {
			RaceGame.togglePause()
			RaceGame.CurrentAction = C.Start
			break
		}
	case C.GAME_OVER_STATE:
		enemies = nil
		if RaceGame.Info.score > RaceGame.Info.high_score {
			RaceGame.Info.high_score = RaceGame.Info.score
			saveHighScore(int(RaceGame.Info.high_score))
		}
		if RaceGame.CurrentAction == C.Start {
			RaceGame.State = C.START_STATE
		}
	}
	if RaceGame.State != C.GAME_OVER_STATE {
		switch RaceGame.CurrentAction {
		case C.Left, C.Right, C.Terminate, C.Pause, C.Up, C.Down, C.Action:
			RaceGame.CurrentAction = C.Start
		}
	}
}

func tick() {
	now := updateTime()
	speedMs := int64(RaceGame.EnemySpeed / time.Millisecond)
	if RaceGame.Holding {
		speedMs /= 2
	}
	if now-RaceGame.Timer >= speedMs {
		RaceGame.moveEnemies()
		RaceGame.Timer = now
	}
	spawnInterval := getSpawnInterval(RaceGame.Info.level)
	if now-RaceGame.SpawnTimer >= spawnInterval {
		spawnEnemy()
		RaceGame.SpawnTimer = now
	}
}

func (g *RaceGameInfo) togglePause() {
	if g.Info.pause == 0 {
		g.Info.pause = 1
		g.State = C.PAUSE_STATE
	} else {
		g.Info.pause = 0
		g.State = C.MOVING_STATE
		now := updateTime()
		g.Timer = now
		g.SpawnTimer = now
	}
}

func setEnemyCarOnNext() {
	if NextArea == nil {
		return
	}

	for i := 0; i < int(MAX_FIG_ROWS); i++ {
		for j := 0; j < int(MAX_FIG_COLS); j++ {
			*NextArea[i][j] = EMPTY_BLOCK
		}
	}

	const yOffset = 1

	for i := 0; i < CAR_HEIGHT; i++ {
		for j := 0; j < CAR_WIDTH; j++ {
			newI := j + yOffset
			newJ := CAR_HEIGHT - 1 - i

			if newI < int(MAX_FIG_ROWS) && newJ < int(MAX_FIG_COLS) {
				block := enemyCar[i][j]
				if block == ENEMY_BLOCK {
					block = PLAYER_BLOCK
				}
				*NextArea[newI][newJ] = block
			}
		}
	}
}

func getSpawnInterval(level C.int) int64 {
	base := 3000 - int(level)*200
	if base < 800 {
		base = 800
	}
	variance := int64(float64(base) * 0.2)
	minInterval := base - int(variance)
	maxInterval := base + int(variance)
	if minInterval < 500 {
		minInterval = 500
	}
	return int64(rand.Intn(maxInterval-minInterval+1) + minInterval)
}

func loadHighScore() int {
	data, err := os.ReadFile("race_high_score.txt")
	if err != nil {
		saveHighScore(0)
		return 0
	}
	score, err := strconv.Atoi(string(data))
	if err != nil {
		saveHighScore(0)
		return 0
	}
	return score
}

func saveHighScore(score int) {
	err := os.WriteFile("race_high_score.txt", []byte(strconv.Itoa(score)), 0o644)
	if err != nil {
		panic("creating high scores file error")
	}
}

func main() {}

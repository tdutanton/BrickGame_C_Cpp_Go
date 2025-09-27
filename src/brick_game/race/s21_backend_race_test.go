package main

import (
	"testing"
	"time"

	"github.com/stretchr/testify/assert"
	"github.com/stretchr/testify/require"
)

func setupGame(t *testing.T) *RaceGameInfo {
	t.Helper()
	game := InitNewGame()
	require.NotNil(t, game)
	return game
}

func teardownGame() {
	DestroyGame()
}

func TestFSM_StartToSpawn(t *testing.T) {
	game := setupGame(t)
	defer teardownGame()

	assert.Equal(t, int(START_STATE), int(game.State))

	userInput(Start, false)
	updateCurrentState()

	assert.Equal(t, int(SPAWN_STATE), int(RaceGame.State))
}

func TestFSM_SpawnToMoving(t *testing.T) {
	game := setupGame(t)
	defer teardownGame()

	game.State = SPAWN_STATE
	updateCurrentState()

	assert.Equal(t, int(MOVING_STATE), int(RaceGame.State))
}

func TestHandleMovement_LeftRight(t *testing.T) {
	game := setupGame(t)
	defer teardownGame()

	startX := game.Player.X

	userInput(Left, false)
	game.handleMovement()
	assert.Equal(t, startX-1, game.Player.X)

	userInput(Right, false)
	game.handleMovement()
	assert.Equal(t, startX, game.Player.X)
}

func TestSpawnEnemy(t *testing.T) {
	setupGame(t)
	defer teardownGame()

	assert.Equal(t, 0, len(enemies))

	spawnEnemy()
	assert.Equal(t, 1, len(enemies))
}

func TestMoveEnemies(t *testing.T) {
	game := setupGame(t)
	defer teardownGame()

	enemies = nil
	spawnEnemy()
	require.NotEmpty(t, enemies)

	initialY := enemies[0].Y
	game.moveEnemies()
	assert.Equal(t, initialY+1, enemies[0].Y)
}

func TestUpdateLevel(t *testing.T) {
	game := setupGame(t)
	defer teardownGame()

	assert.Equal(t, 1, int(game.Info.level))

	game.Info.score = 5
	game.updateLevel()
	assert.Equal(t, 2, int(game.Info.level))
}

func TestCheckCollision_NoCollision(t *testing.T) {
	game := setupGame(t)
	defer teardownGame()

	enemies = nil
	spawnEnemy()
	enemies[0].Y = int(ROWS)

	collided := checkCollision()
	assert.False(t, collided)
	assert.NotEqual(t, int(GAME_OVER_STATE), int(game.State))
}

func TestFSM_PauseAndResume(t *testing.T) {
	game := setupGame(t)
	defer teardownGame()

	game.State = MOVING_STATE
	userInput(Pause, false)
	updateCurrentState()
	assert.Equal(t, int(PAUSE_STATE), int(RaceGame.State))

	userInput(Pause, false)
	updateCurrentState()
	assert.Equal(t, int(MOVING_STATE), int(RaceGame.State))
}

func TestFSM_GameOverAndRestart(t *testing.T) {
	game := setupGame(t)
	defer teardownGame()

	game.Info.score = 100
	game.Info.high_score = 50
	game.State = GAME_OVER_STATE
	updateCurrentState()

	assert.Equal(t, 100, int(game.Info.high_score))

	userInput(Start, false)
	updateCurrentState()

	assert.Equal(t, 0, int(RaceGame.Info.score))
}

func TestCanSpawnAt(t *testing.T) {
	setupGame(t)
	defer teardownGame()
	enemies = []Car{{X: 2, Y: 0}}

	assert.False(t, canSpawnAt(2, 0))

	assert.True(t, canSpawnAt(5, 0))
}

func TestHasSafePath(t *testing.T) {
	player := Car{X: 5, Y: 10}
	enemies := []Car{
		{X: 4, Y: 9},
		{X: 5, Y: 9},
		{X: 6, Y: 9},
	}

	assert.False(t, hasSafePath(player, enemies))
}

func TestGetSpawnInterval(t *testing.T) {
	assert.Equal(t, int64(3000), getSpawnInterval(0))
	assert.Equal(t, int64(2800), getSpawnInterval(1))
	assert.Equal(t, int64(800), getSpawnInterval(12))
	assert.Equal(t, int64(800), getSpawnInterval(20))
}

func TestUpdateLevel_Cap(t *testing.T) {
	game := setupGame(t)
	defer teardownGame()

	game.Info.level = 10
	game.Info.score = 50
	game.updateLevel()

	assert.Equal(t, 10, int(game.Info.level))
	assert.Equal(t, 500*time.Millisecond, game.EnemySpeed)
}

func TestDrawObjects(t *testing.T) {
	game := setupGame(t)
	defer teardownGame()

	spawnEnemy()
	require.NotEmpty(t, enemies)

	clearField()
	drawObjects()

	playerX := game.Player.X
	playerY := game.Player.Y
	assert.Equal(t, int(PLAYER_BLOCK), int(*FieldArea[playerY][playerX+1]))

	enemyX := enemies[0].X
	enemyY := enemies[0].Y
	assert.Equal(t, int(ENEMY_BLOCK), int(*FieldArea[enemyY][enemyX+1]))
}

func TestDestroyGame(t *testing.T) {
	setupGame(t)
	ptr := fullInfoPtr
	require.NotNil(t, ptr)

	DestroyGame()
	assert.Nil(t, RaceGame)
	assert.Nil(t, fullInfoPtr)
	assert.Nil(t, FieldArea)
	assert.Nil(t, NextArea)
}

func TestIsPlayerPathBlocked(t *testing.T) {
	setupGame(t)
	defer teardownGame()

	playerY := RaceGame.Player.Y
	for x := 0; x <= int(COLS)-CAR_WIDTH; x += CAR_WIDTH {
		enemies = append(enemies, Car{X: x, Y: playerY})
	}

	blocked := isPlayerPathBlocked()
	assert.True(t, blocked)

	enemies = enemies[:len(enemies)-1]
	blocked = isPlayerPathBlocked()
	assert.False(t, blocked)
}

func TestSpawnEnemy_MaxAttempts(t *testing.T) {
	setupGame(t)
	defer teardownGame()

	for x := 0; x <= int(COLS)-CAR_WIDTH; x += CAR_WIDTH {
		enemies = append(enemies, Car{X: x, Y: 0})
	}

	initialCount := len(enemies)
	spawnEnemy()
	assert.Equal(t, initialCount, len(enemies))
}

func TestCheckCollision_WithCollision(t *testing.T) {
	setupGame(t)
	defer teardownGame()

	enemies = []Car{{
		X: RaceGame.Player.X,
		Y: RaceGame.Player.Y,
	}}

	collided := checkCollision()
	assert.True(t, collided)
}

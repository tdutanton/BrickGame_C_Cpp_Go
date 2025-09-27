package race

/*
#cgo LDFLAGS: ${SRCDIR}/../../../s21_race.so
#include "../../race/s21_backend_race.h"
*/
import "C"

import (
	server "brickGameRace/brick_game/server"
	"fmt"
	"unsafe"
)

type RaceGame struct {
	server.BaseGame
}

func NewRaceGame() *RaceGame {
	return &RaceGame{}
}

func (g *RaceGame) Start() {
	g.BaseGame.Mu.Lock()
	defer g.BaseGame.Mu.Unlock()
	C.userInput(C.Start, 0)
	C.updateCurrentState()
	g.Running = true
}

func (g *RaceGame) HandleAction(actionID int, hold bool) {
	g.BaseGame.Mu.Lock()
	defer g.BaseGame.Mu.Unlock()
	holdVal := C.uchar(0)
	if hold {
		holdVal = 1
	}
	C.userInput(C.UserAction_t(actionID), holdVal)
}

func (g *RaceGame) GetState() (server.GameState, error) {
	g.BaseGame.Mu.Lock()
	defer g.BaseGame.Mu.Unlock()
	info := C.updateCurrentState()
	if info.field == nil {
		return server.GameState{}, fmt.Errorf("игра не инициализирована")
	}
	field := g.convertMatrix(info.field, int(C.ROWS), int(C.COLS))
	next := g.convertMatrix(info.next, int(C.MAX_FIG_ROWS), int(C.MAX_FIG_COLS))
	return server.GameState{
		Field:     field,
		Next:      next,
		Score:     int(info.score),
		HighScore: int(info.high_score),
		Level:     int(info.level),
		Speed:     int(info.speed),
		Pause:     info.pause != 0,
	}, nil
}

func (g *RaceGame) convertMatrix(ptr **C.int, rows, cols int) [][]bool {
	if ptr == nil {
		matrix := make([][]bool, rows)
		for i := range matrix {
			matrix[i] = make([]bool, cols)
		}
		return matrix
	}

	rowsPtr := (*[1 << 30]*C.int)(unsafe.Pointer(ptr))[:rows:rows]
	matrix := make([][]bool, rows)
	for i := 0; i < rows; i++ {
		if rowsPtr[i] == nil {
			matrix[i] = make([]bool, cols)
			continue
		}
		rowPtr := (*[1 << 30]C.int)(unsafe.Pointer(rowsPtr[i]))[:cols:cols]
		matrix[i] = make([]bool, cols)
		for j := 0; j < cols; j++ {
			matrix[i][j] = rowPtr[j] != 0
		}
	}
	return matrix
}

func (g *RaceGame) IsRunning() bool {
	g.Mu.Lock()
	defer g.Mu.Unlock()
	return g.Running
}

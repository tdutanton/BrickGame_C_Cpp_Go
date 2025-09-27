package server

import "sync"

// GameInterface - универсальный интерфейс для всех игр
type GameInterface interface {
	Start()
	HandleAction(actionID int, hold bool)
	GetState() (GameState, error)
	IsRunning() bool
}

// Базовая структура для всех игр
type BaseGame struct {
	Mu      sync.Mutex
	Running bool
}

func (g *BaseGame) IsRunning() bool {
	g.Mu.Lock()
	defer g.Mu.Unlock()
	return g.Running
}

package server

import "fmt"

type GameFactory struct {
	games map[int]GameEntry
}

type GameEntry struct {
	Info GameInfo
	Game GameInterface
}

func NewGameFactory() *GameFactory {
	return &GameFactory{
		games: make(map[int]GameEntry),
	}
}

func (f *GameFactory) RegisterGame(gameID int, name string, game GameInterface) {
	f.games[gameID] = GameEntry{
		Info: GameInfo{ID: gameID, Name: name},
		Game: game,
	}
}

func (f *GameFactory) GetGame(gameID int) (GameInterface, error) {
	entry, exists := f.games[gameID]
	if !exists {
		return nil, fmt.Errorf("игра с ID %d не найдена", gameID)
	}
	return entry.Game, nil
}

func (f *GameFactory) GetGameInfo(gameID int) (GameInfo, error) {
	entry, exists := f.games[gameID]
	if !exists {
		return GameInfo{}, fmt.Errorf("игра с ID %d не найдена", gameID)
	}
	return entry.Info, nil
}

func (f *GameFactory) GetAvailableGames() []GameInfo {
	result := make([]GameInfo, 0, len(f.games))
	for _, entry := range f.games {
		result = append(result, entry.Info)
	}
	return result
}

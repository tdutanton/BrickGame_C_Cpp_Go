package server

type GameInfo struct {
	ID   int    `json:"id"`
	Name string `json:"name"`
}

type GamesList struct {
	Games []GameInfo `json:"games"`
}

type UserAction struct {
	ActionID int  `json:"actionId"`
	Hold     bool `json:"hold"`
}

type GameState struct {
	Field     [][]bool `json:"field"`
	Next      [][]bool `json:"next"`
	Score     int      `json:"score"`
	HighScore int      `json:"high_score"`
	Level     int      `json:"level"`
	Speed     int      `json:"speed"`
	Pause     bool     `json:"pause"`
}

type ErrorMessage struct {
	Message string `json:"message"`
}

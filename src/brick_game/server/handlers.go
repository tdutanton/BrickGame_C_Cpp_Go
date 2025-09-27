package server

import (
	"fmt"
	"net/http"
	"strconv"

	"github.com/gin-gonic/gin"
)

func GetGameFactory() *GameFactory {
	return gameFactoryInstance
}

var gameFactoryInstance *GameFactory

func InitGameFactory() {
	gameFactoryInstance = NewGameFactory()
}

func GetGames(c *gin.Context) {
	resp := GamesList{
		Games: gameFactoryInstance.GetAvailableGames(),
	}
	c.JSON(http.StatusOK, resp)
}

func StartGame(c *gin.Context) {
	id, err := strconv.Atoi(c.Param("gameId"))
	if err != nil {
		c.JSON(http.StatusBadRequest, ErrorMessage{Message: "Некорректный ID"})
		return
	}

	game, err := gameFactoryInstance.GetGame(id)
	if err != nil {
		c.JSON(http.StatusNotFound, ErrorMessage{Message: err.Error()})
		return
	}

	if currentGame, exists := getCurrentGame(); exists && currentGame.IsRunning() {
		currentGameInstance = nil
		hasCurrentGame = false
	}

	game.Start()
	setCurrentGame(game)

	info, _ := gameFactoryInstance.GetGameInfo(id)
	c.JSON(http.StatusOK, gin.H{"message": fmt.Sprintf("Игра %s запущена", info.Name)})
}

func DoAction(c *gin.Context) {
	var action UserAction
	if err := c.ShouldBindJSON(&action); err != nil {
		c.JSON(http.StatusBadRequest, ErrorMessage{Message: "Ошибка в теле запроса"})
		return
	}

	game, exists := getCurrentGame()
	if !exists || !game.IsRunning() {
		c.JSON(http.StatusBadRequest, ErrorMessage{Message: "Пользователь не запустил игру"})
		return
	}

	game.HandleAction(action.ActionID, action.Hold)
	c.JSON(http.StatusOK, gin.H{"message": "Действие выполнено"})
}

func GetState(c *gin.Context) {
	game, exists := getCurrentGame()
	if !exists || !game.IsRunning() {
		c.JSON(http.StatusBadRequest, ErrorMessage{Message: "Пользователь не запустил игру"})
		return
	}

	state, err := game.GetState()
	if err != nil {
		c.JSON(http.StatusBadRequest, ErrorMessage{Message: err.Error()})
		return
	}
	c.JSON(http.StatusOK, state)
}

var currentGameInstance GameInterface
var hasCurrentGame bool

func getCurrentGame() (GameInterface, bool) {
	return currentGameInstance, hasCurrentGame
}

func setCurrentGame(game GameInterface) {
	if game == nil {
		currentGameInstance = nil
		hasCurrentGame = false
		return
	}
	currentGameInstance = game
	hasCurrentGame = true
}

func StopGame(c *gin.Context) {
	if currentGameInstance != nil {
		currentGameInstance = nil
		hasCurrentGame = false
	}
	c.JSON(http.StatusOK, gin.H{"message": "Игра остановлена"})
}

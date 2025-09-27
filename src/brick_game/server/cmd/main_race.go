//go:build race_game

package main

import (
	server "brickGameRace/brick_game/server"
	race "brickGameRace/brick_game/server/race"

	"github.com/gin-contrib/cors"
	"github.com/gin-gonic/gin"
)

func main() {
	server.InitGameFactory()
	gameFactory := server.GetGameFactory()
	gameFactory.RegisterGame(1, "Race", race.NewRaceGame())

	r := gin.Default()
	r.Use(cors.Default())

	api := r.Group("/api")
	{
		api.GET("/games", server.GetGames)
		api.POST("/games/:gameId", server.StartGame)
		api.POST("/actions", server.DoAction)
		api.GET("/state", server.GetState)
		api.POST("/stop", server.StopGame)
	}

	r.StaticFS("/static", gin.Dir("gui/web_gui", false))
	r.GET("/", func(c *gin.Context) {
		c.File("gui/web_gui/index.html")
	})

	r.Run(":8080")
}

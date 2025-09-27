import { applyRootStyles } from './src/utils.js';
import { GameBoard } from './src/game-board.js';
import { rootStyles, keyCodes } from './src/config.js';

applyRootStyles(rootStyles);

const $gameSelector = document.getElementById('game-selector');
const $gameScreen = document.getElementById('game-screen');
const $gamesList = document.getElementById('games-list');
const $gameBoard = document.getElementById('game-board');
const $score = document.getElementById('score-value');
const $highScore = document.getElementById('high-score-value');
const $level = document.getElementById('level-value');
const $speed = document.getElementById('speed-value');
const $nextFigure = document.getElementById('next-figure');
const $pauseOverlay = document.getElementById('pause-overlay');
const $startBtn = document.getElementById('start-btn');
const $backBtn = document.getElementById('back-btn');

let gameBoard = null;
let currentGameId = null;
let isHeld = false;
let gameInterval = null;

function createNextFigureGrid() {
    $nextFigure.innerHTML = '';
    for (let i = 0; i < 4; i++) {
        for (let j = 0; j < 4; j++) {
            const tile = document.createElement('div');
            tile.className = 'next-tile';
            tile.dataset.row = i;
            tile.dataset.col = j;
            $nextFigure.appendChild(tile);
        }
    }
}

createNextFigureGrid();

async function loadGames() {
    try {
        const res = await fetch('/api/games');
        const data = await res.json();

        $gamesList.innerHTML = '';
        data.games.forEach(game => {
            const btn = document.createElement('button');
            btn.className = 'game-btn';
            btn.textContent = game.name;
            btn.dataset.gameId = game.id;
            btn.addEventListener('click', () => selectGame(game.id));
            $gamesList.appendChild(btn);
        });
    } catch (error) {
        console.error('Ошибка загрузки списка игр:', error);
    }
}

function selectGame(gameId) {
    currentGameId = gameId;
    $gameSelector.classList.add('hidden');
    $gameScreen.classList.remove('hidden');

    $gameBoard.innerHTML = '';
    gameBoard = new GameBoard($gameBoard, 10, 20);
}

$backBtn.addEventListener('click', () => {
    if (gameInterval) {
        clearInterval(gameInterval);
        gameInterval = null;
    }
    currentGameId = null;
    $gameScreen.classList.add('hidden');
    $gameSelector.classList.remove('hidden');
});

$startBtn.addEventListener('click', async () => {
    if (!currentGameId) {
        console.error('Игра не выбрана');
        return;
    }

    try {
        const res = await fetch(`/api/games/${currentGameId}`, { method: 'POST' });
        const data = await res.json();
        console.log('Игра запущена:', data);

        if (gameInterval) clearInterval(gameInterval);
        gameInterval = setInterval(updateGameState, 200);
    } catch (error) {
        console.error('Ошибка запуска игры:', error);
    }
});

async function updateGameState() {
    try {
        const res = await fetch('/api/state');
        const state = await res.json();

        gameBoard.clearAllTiles();
        for (let y = 0; y < state.field.length; y++) {
            for (let x = 0; x < state.field[y].length; x++) {
                if (state.field[y][x]) {
                    gameBoard.enableTile(x, y);
                }
            }
        }

        $score.textContent = state.score || 0;
        $highScore.textContent = state.high_score || 0;
        $level.textContent = state.level || 1;
        $speed.textContent = state.speed || 500;

        updateNextFigure(state.next);

        if (state.pause) {
            $pauseOverlay.classList.remove('hidden');
        } else {
            $pauseOverlay.classList.add('hidden');
        }
    } catch (error) {
        console.error('Ошибка при обновлении состояния:', error);
    }
}

function updateNextFigure(nextMatrix) {
    for (let i = 0; i < 4; i++) {
        for (let j = 0; j < 4; j++) {
            const tile = $nextFigure.querySelector(`[data-row="${i}"][data-col="${j}"]`);
            if (tile) {
                if (nextMatrix[i] && nextMatrix[i][j]) {
                    tile.classList.add('active');
                } else {
                    tile.classList.remove('active');
                }
            }
        }
    }
}

document.addEventListener('keydown', async (event) => {
    if (!currentGameId || $gameScreen.classList.contains('hidden')) {
        return;
    }

    if (!event.code || event.code === 'Unidentified' || event.code === 'Process') {
        return;
    }

    let actionId = null;
    if (keyCodes.up.includes(event.code)) {
        actionId = 5;
        if (!isHeld) {
            isHeld = true;
        }
    }
    else if (keyCodes.right.includes(event.code)) actionId = 4;
    else if (keyCodes.down.includes(event.code)) actionId = 6;
    else if (keyCodes.left.includes(event.code)) actionId = 3;
    else if (event.code === 'Space') {
        actionId = 7;
        if (!isHeld) {
            isHeld = true;
        }
    }
    else if (event.code === 'KeyP') actionId = 0;

    if (actionId !== null) {
        console.log("Отправляем actionId:", actionId, "hold:", isHeld);
        try {
            await fetch('/api/actions', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ actionID: actionId, hold: isHeld })
            });
        } catch (error) {
            console.error('Ошибка при отправке действия:', error);
        }
    }
});

document.addEventListener('keyup', async (event) => {
    if (!currentGameId || $gameScreen.classList.contains('hidden')) {
        return;
    }

    let actionId = null;
    if (keyCodes.up.includes(event.code)) {
        actionId = 5;
        if (isHeld) {
            isHeld = false;
        }
    }
    else if (event.code === 'Space') {
        actionId = 7;
        if (isHeld) {
            isHeld = false;
        }
    }

    if (actionId !== null) {
        console.log("Отправляем actionId:", actionId, "hold:", isHeld);
        try {
            await fetch('/api/actions', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ actionID: actionId, hold: isHeld })
            });
        } catch (error) {
            console.error('Ошибка при отправке действия:', error);
        }
    }
});

loadGames();
export class GameBoard {
    constructor($gameBoard, width = 10, height = 20) {
        this.width = width;
        this.height = height;
        this.element = $gameBoard;
        this.tiles = [];

        this.element.style.gridTemplateColumns = `repeat(${width}, var(--tile-size, 20px))`;
        this.element.style.gridTemplateRows = `repeat(${height}, var(--tile-size, 20px))`;

        for (let y = 0; y < height; y++) {
            for (let x = 0; x < width; x++) {
                const $tile = document.createElement('div');
                $tile.classList.add('tile');
                $tile.dataset.x = x;
                $tile.dataset.y = y;
                this.tiles.push($tile);
                this.element.append($tile);
            }
        }
    }

    getTile(x, y) {
        if (x < 0 || x >= this.width || y < 0 || y >= this.height) {
            return null;
        }
        return this.tiles[y * this.width + x];
    }

    enableTile(x, y) {
        const tile = this.getTile(x, y);
        if (tile) tile.classList.add('active');
    }

    disableTile(x, y) {
        const tile = this.getTile(x, y);
        if (tile) tile.classList.remove('active');
    }

    clearAllTiles() {
        this.tiles.forEach(t => t.classList.remove('active'));
    }
}
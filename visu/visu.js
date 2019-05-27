$(() => {

	let responsivePuzzle = (screenW, screenH, puzzleSize) => {
		return screenW < screenH ? Math.round(screenW / puzzleSize) : Math.round(screenH / puzzleSize);   
	};

	var refreshIntervalP;

	let configuration = {
		currentState: 0,
		puzzleStates: puzzleStates,
		puzzleSize: size,
		caseSize: responsivePuzzle((window.innerWidth || document.body.clientWidth), (window.innerHeight || document.body.clientHeight), size + 1)
	}

	let buildPuzzle = (config) => {
		let svgSize = config.caseSize * config.puzzleSize;
		let puzzleContent = config.puzzleStates;
		var state = config.currentState;
		var puzzle = `<svg id="puzzle" width="`+ svgSize +`" height="`+ svgSize +`">`;
		for (var i = 0; i < config.puzzleSize; i++) {
			for (var j = 0; j < config.puzzleSize; j++) {
				var fontSize = config.caseSize / 5;
				var index = (i * config.puzzleSize) + j;
				var txt = config.puzzleStates[state][index] == 0 ? '' : config.puzzleStates[state][index];
				var backgroundColor = config.puzzleStates[state][index] != 0 ? '#ebeff1' : '#fff';
				var y = (((i + 1) * config.caseSize) - config.caseSize / 2) + config.caseSize / 15;
				var x = (((j + 1) * config.caseSize) - config.caseSize / 2) - config.caseSize / 15;
				if (config.puzzleStates[state][index] >= 100)
					x -= 20;
				else if (config.puzzleStates[state][index] >= 10)
					x -= 10;
				puzzle += `<g><rect width="`+ config.caseSize +`" height="`+ config.caseSize +`" 
					x="`+ j * config.caseSize +`" y="`+ i * config.caseSize +`" fill="`+ backgroundColor +`" stroke="#000"
					stroke-opacity="0.2"></rect><text font-family="Verdana" fill="#434c5b" font-size="`+ fontSize +`" x="`+ x +`" 
					y="`+ y +`">`+ txt +`</text></g>`;
			}
		}
		puzzle += `</svg>`;
		$(`body`).prepend(puzzle);
	};

	let bPlay = (config) => {
		$(`#play`).off(`click`).on(`click`, () => {
			var i = config.currentState;
			refreshIntervalP = setInterval(() => {
				if (i < config.puzzleStates.length) {
					$(`#puzzle`).remove();
					$(`#cnt`).remove();
					configuration.currentState = i++;
					buildPuzzle(config, i);
					buildCnt(config);
				}
			}, 400);
			$(`#play`).remove();
			$(`<button id="refresh">Refresh</button>`).insertBefore(`#next`)
			bRefresh();
		});
	}

	let bRefresh = () => {

		$(`#refresh`).off(`click`).on(`click`, () => {
			clearInterval(refreshIntervalP);
			configuration.currentState = 0;
			$(`#puzzle`).remove();
			$(`#cnt`).remove();
			buildPuzzle(configuration);
			buildCnt(configuration);
			$(`#refresh`).remove();
			$(`<button id="play">Play</button>`).insertBefore(`#next`)
			bPlay(configuration);
		});
	}

	let bPrevious = () => {
		$(`#previous`).off(`click`).on(`click`, () => {
			if (configuration.currentState > 0) {
				configuration.currentState--;
				$(`#puzzle`).remove();
				$(`#cnt`).remove();
				buildPuzzle(configuration);
				buildCnt(configuration);
			}
			clearInterval(refreshIntervalP);
			bPlay(configuration);
		});
	}

	let bNext = () => {
		$(`#next`).off(`click`).on(`click`, () => {
			if (configuration.currentState < configuration.puzzleStates.length - 1) {
				configuration.currentState++;
				$(`#puzzle`).remove();
				$(`#cnt`).remove();
				buildPuzzle(configuration);
				buildCnt(configuration);
			}
			clearInterval(refreshIntervalP);
			bPlay(configuration);
		});
	}

	let initButtons = () => {
		bPlay(configuration);
		bPrevious();
		bNext();
	};

	let buildCnt = (config) => {
		var max = config.puzzleStates.length - 1;
		var current = config.currentState;
		$(`body`).prepend(`<div id="cnt"><h3>`+ current +` / `+ max +`</h3></div>`);
	};

	initButtons();
	buildPuzzle(configuration);
	buildCnt(configuration);
});
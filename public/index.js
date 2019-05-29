window.onload = function() {
  fetch('./data.json').then(res =>
    res.json().then(json => {
      const dividedSize = (sWidth, sHeight, puzzleSize) => {
        return sWidth < sHeight
          ? Math.round(sWidth / puzzleSize)
          : Math.round(sHeight / puzzleSize);
      };

      const puzzle = {
        currState: 0,
        states: json.states,
        size: json.size,
        tileSize: dividedSize(
          $(window).innerWidth() * (2 / 3),
          $(window).innerHeight() * (2 / 3),
          json.size
        )
      };

      $('.card-header-title').html(`Puzzle ${puzzle.size} x ${puzzle.size}`);
      const renderSvg = () => {
        const svgSize = puzzle.tileSize * puzzle.size;
        let stateIdx = puzzle.currState;
        let svg = `<svg id="tiles" style="display: block; margin: auto;" width="${svgSize}" height="${svgSize}">`;
        for (var y = 0; y < puzzle.size; y++) {
          for (var x = 0; x < puzzle.size; x++) {
            const fontSize = puzzle.tileSize / 5;
            const tileIdx = y * puzzle.size + x;
            const nbrText =
              puzzle.states[stateIdx][tileIdx] == 0
                ? ''
                : puzzle.states[stateIdx][tileIdx];
            const bgColor =
              puzzle.states[stateIdx][tileIdx] != 0 ? 'slategrey' : 'white';
            const yPos = y * puzzle.tileSize + puzzle.tileSize / 2;
            const xPos = x * puzzle.tileSize + puzzle.tileSize / 2;
            svg +=
              `<g><rect width="${puzzle.tileSize}" height="${
                puzzle.tileSize
              }" x="${x * puzzle.tileSize}" y="${y *
                puzzle.tileSize}" fill="${bgColor}" fill-opacity="0.1" stroke="black" stroke-opacity="0.2"></rect>` +
              `<text text-anchor="middle" dominant-baseline="central" font-family="Helvetica" fill="#363636" font-size="${fontSize}" x="${xPos}" y="${yPos}">${nbrText}</text></g>`;
          }
        }
        svg += '</svg>';
        $('.card-content:first').append(svg);
      };

      const changeIcon = (oldIcon, newIcon) => {
        $('#toggle-animation i').removeClass(`fa-${oldIcon}`);
        $('#toggle-animation i').addClass(`fa-${newIcon}`);
      };

      let animInterval;
      function playAnim() {
        animInterval = setInterval(() => {
          if (puzzle.currState++ < puzzle.states.length - 1) {
            $('#tiles').remove();
            renderSvg();
          } else clearInterval(animInterval);
        }, 400);
        changeIcon('play', 'pause');
        $(this).one('click', pauseAnim);
      }
      function pauseAnim() {
        changeIcon('pause', 'play');
        clearInterval(animInterval);
        $(this).one('click', playAnim);
      }
      $('#toggle-animation').one('click', playAnim);

      $(`#refresh-puzzle`).on(`click`, () => {
        if (puzzle.currState === 0) return;
        clearInterval(animInterval);
        puzzle.currState = 0;
        $(`#tiles`).remove();
        renderSvg();
        changeIcon('pause', 'play');
        $('#toggle-animation').off('click');
        $('#toggle-animation').one('click', playAnim);
      });

      renderSvg();
    })
  );
};

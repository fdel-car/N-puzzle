window.onload = function() {
  fetch('./data.json').then(res => {
    if (res.ok)
      res.json().then(json => {
        const dividedSize = (sWidth, sHeight) => {
          return sWidth < sHeight
            ? Math.round(Math.max(sWidth, 276) / json.size)
            : Math.round(Math.max(sHeight, 276) / json.size);
        };

        let puzzle = {
          currState: 0,
          tileSize: dividedSize(
            $(window).innerWidth() * (2 / 3),
            $(window).innerHeight() * (2 / 3)
          )
        };

        $('.card-header-title').html(`Puzzle ${json.size} x ${json.size}`);
        const renderSvg = () => {
          const svgSize = puzzle.tileSize * json.size;
          let stateIdx = puzzle.currState;
          if (renderSvg.stateIdx !== stateIdx) updateStateIdx();
          let svg = `<svg id="tiles" width="${svgSize}" height="${svgSize}">`;
          for (var y = 0; y < json.size; y++) {
            for (var x = 0; x < json.size; x++) {
              const fontSize = puzzle.tileSize / 5;
              const tileIdx = y * json.size + x;
              const nbrText =
                json.states[stateIdx][tileIdx] == 0
                  ? ''
                  : json.states[stateIdx][tileIdx];
              const bgColor =
                json.states[stateIdx][tileIdx] != 0 ? 'slategrey' : 'white';
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
          $(svg).insertBefore('#help');
        };

        const changeIcon = (oldIcon, newIcon) => {
          $('#toggle-animation i').removeClass(`fa-${oldIcon}`);
          $('#toggle-animation i').addClass(`fa-${newIcon}`);
        };

        const updateStateIdx = () => {
          renderSvg.stateIdx = puzzle.currState;
          $('#state-idx').text(
            `${puzzle.currState + 1} / ${json.states.length}.`
          );
        };

        let animInterval;
        function playAnim() {
          animInterval = setInterval(() => {
            if (puzzle.currState < json.states.length - 1) {
              puzzle.currState++;
              $('#tiles').remove();
              renderSvg();
            } else pauseAnim();
          }, 200);
          changeIcon('play', 'pause');
          $(this)
            .off('click')
            .one('click', pauseAnim);
        }
        function pauseAnim() {
          changeIcon('pause', 'play');
          clearInterval(animInterval);
          $('#toggle-animation')
            .off('click')
            .one('click', playAnim);
        }
        $('#toggle-animation').one('click', playAnim);

        $(`#refresh-puzzle`).on(`click`, () => {
          if (puzzle.currState === 0) return;
          pauseAnim();
          puzzle.currState = 0;
          $(`#tiles`).remove();
          renderSvg();
        });

        $(`#prev-state`).on(`click`, () => {
          if (puzzle.currState <= 0) return;
          pauseAnim();
          puzzle.currState--;
          $(`#tiles`).remove();
          renderSvg();
        });

        $(`#next-state`).on(`click`, () => {
          if (puzzle.currState >= json.states.length - 1) return;
          pauseAnim();
          puzzle.currState++;
          $(`#tiles`).remove();
          renderSvg();
        });

        const setCardSize = () => {
          $('.card').css(
            'width',
            `calc(${puzzle.tileSize * json.size}px + 1.5rem)`
          );
        };

        function resizeAll() {
          const oldTileSize = puzzle.tileSize;
          puzzle.tileSize = dividedSize(
            $(window).innerWidth() * (2 / 3),
            $(window).innerHeight() * (2 / 3),
            json.size
          );
          if (oldTileSize === puzzle.tileSize) return;
          $(`#tiles`).remove();
          renderSvg();
          setCardSize();
        }

        let resizeInterval;
        $(window).resize(function() {
          clearTimeout(resizeInterval);
          resizeInterval = setTimeout(resizeAll, 60);
        });

        $('#search-time').text(
          `${json.searchTime.toFixed(4).toLocaleString('fr-FR')} sec.`
        );
        $('#time-complexity').text(
          `${json.timeComplexity.toLocaleString('fr-FR')}.`
        );
        $('#size-complexity').text(
          `${json.sizeComplexity.toLocaleString('fr-FR')}.`
        );
        updateStateIdx();
        renderSvg();
        setCardSize();
      });
  });
};

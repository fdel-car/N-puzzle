# N-puzzle

The goal of this project is to solve the N-puzzle ("taquin" in French) game using the A* search algorithm.

## How to use it?

```
Usage: ./n_puzzle [-h] [-H | -m | -l] [-s | -c] [-a | -g | -u] [file]

Goal patterns:
  -s, --snail                 Set the snail solution as the final grid state (default).
  -c, --classic               Use the classic solution, an ordered grid with a trailing 0.

Available heuristics:
  -H, --hamming-distance      Use Hamming distance, corresponds to the number of misplaced tiles, slowest heuristic.
  -m, --manhattan-distance    Use Manhattan distance, also called taxicab metric.
  -l, --linear-conflict       Use linear conflict along with Manhattan distance, fastest heuristic (default).

Type of algorithms:
  -A, --a-star                Both the expected and the past cost are considered during the search (default).
  -g, --greedy                The past cost will be ignored, only considering expected cost, fastest algorithm but the result won't be optimal.
  -u, --uniform-cost          All states will have the same expected cost (discard heuristic choice), so all nodes will be considered, slowest algorithm.

  -h, --help                  Show this help list.
```

The program will wait for the standard input if launched without a file, so the easiest way to try it out is to use the puzzle generator inside the `resources` folder.

```
python resources/generator.py 3 -s | ./n_puzzle
```

## See the result

Once you clone this repo just run `npm install` once you're inside it.
Then, whenever you want you can start the webserver with `npm start` and access `localhost:8080/` to visualize the found solution.

#!/usr/bin/env python

import sys
import errno
import argparse
import random


def makePuzzle(s, solvable, iterations, goalType):
    def swapEmptyTile(p):
        zeroIdx = p.index(0)
        validIndexes = []
        if zeroIdx % s > 0:
            validIndexes.append(zeroIdx - 1)
        if zeroIdx % s < s - 1:
            validIndexes.append(zeroIdx + 1)
        if zeroIdx / s > 0:
            validIndexes.append(zeroIdx - s)
        if zeroIdx / s < s - 1:
            validIndexes.append(zeroIdx + s)
        swapIdx = random.choice(validIndexes)
        p[zeroIdx] = p[swapIdx]
        p[swapIdx] = 0

    p = makeGoal(s, goalType)
    for _ in range(iterations):
        swapEmptyTile(p)
    if not solvable:
        if p[0] == 0 or p[1] == 0:
            p[-1], p[-2] = p[-2], p[-1]
        else:
            p[0], p[1] = p[1], p[0]
    return p


def makeGoal(s, goalType):
    totalSize = s * s
    puzzle = [-1] * totalSize
    if (goalType == "classic"):
        i = 0
        while (i < totalSize - 1):
            puzzle[i] = i + 1
            i += 1
        puzzle[totalSize - 1] = 0
    elif (goalType == "snail"):
        curr = 1
        x = 0
        ix = 1
        y = 0
        iy = 0
        while True:
            puzzle[x + y * s] = curr
            if curr == 0:
                break
            curr += 1
            if x + ix == s or x + ix < 0 or (ix != 0 and puzzle[x + ix + y * s] != -1):
                iy = ix
                ix = 0
            elif y + iy == s or y + iy < 0 or (iy != 0 and puzzle[x + (y + iy) * s] != -1):
                ix = -iy
                iy = 0
            x += ix
            y += iy
            if curr == totalSize:
                curr = 0
    return puzzle


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "size", type=int, help="size of the puzzle's side (must be >= 3)")
    solvability = parser.add_mutually_exclusive_group()
    solvability.add_argument("-s", "--solvable", action="store_true")
    solvability.add_argument("-u", "--unsolvable", action="store_true")
    parser.add_argument("-i", "--iterations", type=int, default=1000,
                        help="number of random empty tile swap made to the goal state")
    parser.add_argument(
        "-g", "--goal-type", choices=["snail", "classic"], default="snail")
    args = parser.parse_args()

    random.seed()
    if args.size < 3:
        print "The size of the puzzle's side must be >= 3."
        sys.exit(1)

    if not args.solvable and not args.unsolvable:
        args.solvable = random.choice([True, False])

    s = args.size
    puzzle = makePuzzle(s, solvable=args.solvable,
                        iterations=args.iterations, goalType=args.goal_type)

    w = len(str(s * s))
    try:
        print "# This puzzle is %s" % (
            "solvable" if args.solvable else "unsolvable")
        print "%d" % s
        for y in range(s):
            for x in range(s):
                print "%s" % (str(puzzle[x + y * s]).rjust(w)),
            print
    except IOError as err:
        pass

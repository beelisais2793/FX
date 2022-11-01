#!/usr/bin/env python

# Quick and dirty plotting tool


# Let printing work the same in Python 2 and 3
from __future__ import print_function
# Import required modules in this notebook
import numpy as np
import matplotlib
matplotlib.rc('legend', fontsize=10)
import matplotlib.pyplot as plt
import csv
import argparse

# Parse arguments
parser = argparse.ArgumentParser(description='Plot graphs of given csv files')
parser.add_argument('files', metavar='N', type=str, nargs='+',
                    help='file paths for plotting')
parser.add_argument('-q', help='Plot average quality', action='store_true', required=False)
parser.add_argument('-Q', help='Plot maximum quality', action='store_true', required=False)
parser.add_argument('-f', help='Plot average fitness', action='store_true', required=False)
parser.add_argument('-F', help='Plot maximum fitness', action='store_true', required=False)
parser.add_argument('-d', help='Plot diversity', action='store_true', required=False)
parser.add_argument('-m', help='Mutation probability', action='store_true', required=False)

args = parser.parse_args()

allcolumns = [False, args.q, args.Q, args.f, args.F, args.d, args.m]
alllegends = ['generation', 'average quality', 'max quality', 'average fitness', 'max fitness',
              'diversity', 'mutation probability']
allmarkers = ['*', '_', '--','-', '-.', ':', '.']

if len(args.files) == 4:
    fig, axes = plt.subplots(2, 2, sharex=True, sharey=True)
    legends = []
    markers = []
    for q, c in enumerate(allcolumns):
        if c:
            legends.append(alllegends[q])
            markers.append(allmarkers[q])

    for i, v in enumerate(("a", "b", "c", "d")):
        values = list()
        for c in allcolumns:
            if c:
                values.append(list())
        with open(args.files[i]) as sv:
            for r, row in enumerate(csv.reader(sv, delimiter=";")):
                if r == 0:
                    continue
                j = 0
                for k, c in enumerate(allcolumns):
                    if c:
                        values[j].append(float(row[k]))
                        j += 1
        ax = axes[i//2, i%2]
        ax.spines['right'].set_visible(False)
        ax.spines['top'].set_visible(False)
        ax.xaxis.set_ticks_position('bottom')
        ax.yaxis.set_ticks_position('left')
        if v == "c" or v == "d":
            ax.set_xlabel('Generations')
        x = range(0, len(values[0]))
        lines = []
        for m in range(0, len(markers)):
            lines.append(*ax.plot(x, values[m], markers[m]))

        ax.text(.5, .9,'(' + v + ')',
        horizontalalignment='center',
        weight="bold",
        transform=ax.transAxes)
    fig.legend(lines, legends)
    plt.show()

elif len(args.files) == 3:
    fig, axes = plt.subplots(3, 1, sharey=True)
    legends = []
    markers = []
    for q, c in enumerate(allcolumns):
        if c:
            legends.append(alllegends[q])
            markers.append(allmarkers[q])

    for i, v in enumerate(("a", "b", "c")):
        values = list()
        for c in allcolumns:
            if c:
                values.append(list())
        with open(args.files[i]) as sv:
            for r, row in enumerate(csv.reader(sv, delimiter=";")):
                if r == 0:
                    continue
                j = 0
                for k, c in enumerate(allcolumns):
                    if c:
                        values[j].append(float(row[k]))
                        j += 1
        ax = axes[i]
        ax.spines['right'].set_visible(False)
        ax.spines['top'].set_visible(False)
        ax.xaxis.set_ticks_position('bottom')
        ax.yaxis.set_ticks_position('left')
        if v == "c":
            ax.set_xlabel('Generations')
        x = range(0, len(values[0]))
        lines = []
        for m in range(0, len(markers)):
            lines.append(*ax.plot(x, values[m], markers[m]))

        ax.text(.5, .9,'(' + v + ')',
        horizontalalignment='center',
        weight="bold",
        transform=ax.transAxes)
    fig.legend(lines, legends)
    plt.show()

elif len(args.files) == 2:
    fig, axes = plt.subplots(2, 1, sharey=True)
    legends = []
    markers = []
    for q, c in enumerate(allcolumns):
        if c:
            legends.append(alllegends[q])
            markers.append(allmarkers[q])

    for i, v in enumerate(("a", "b")):
        values = list()
        for c in allcolumns:
            if c:
                values.append(list())
        with open(args.files[i]) as sv:
            for r, row in enumerate(csv.reader(sv, delimiter=";")):
                if r == 0:
                    continue
                j = 0
                for k, c in enumerate(allcolumns):
                    if c:
                        values[j].append(float(row[k]))
                        j += 1
        ax = axes[i]
        ax.spines['right'].set_visible(False)
        ax.spines['top'].set_visible(False)
        ax.xaxis.set_ticks_position('bottom')
        ax.yaxis.set_ticks_position('left')
        if v == "b":
            ax.set_xlabel('Generations')
        x = range(0, len(values[0]))
        lines = []
        for m in range(0, len(markers)):
            lines.append(*ax.plot(x, values[m], markers[m]))

        ax.text(.5, .9,'(' + v + ')',
        horizontalalignment='center',
        weight="bold",
        transform=ax.transAxes)
    fig.legend(lines, legends)
    plt.show()

elif len(args.files) == 1:
    fig, ax = plt.subplots(1, 1)
    legends = []
    markers = []
    for q, c in enumerate(allcolumns):
        if c:
            legends.append(alllegends[q])
            markers.append(allmarkers[q])

    values = list()
    for c in allcolumns:
        if c:
            values.append(list())
    with open(args.files[0]) as sv:
        for r, row in enumerate(csv.reader(sv, delimiter=";")):
            if r == 0:
                continue
            j = 0
            for k, c in enumerate(allcolumns):
                if c:
                    values[j].append(float(row[k]))
                    j += 1
    ax.spines['right'].set_visible(False)
    ax.spines['top'].set_visible(False)
    ax.xaxis.set_ticks_position('bottom')
    ax.yaxis.set_ticks_position('left')
    ax.set_xlabel('Generations')
    x = range(0, len(values[0]))
    lines = []
    for m in range(0, len(markers)):
        lines.append(*ax.plot(x, values[m], markers[m]))

    fig.legend(lines, legends)
    plt.show()

else:
    print("number of allowed files [1, 4]")


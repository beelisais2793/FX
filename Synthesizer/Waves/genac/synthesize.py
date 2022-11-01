#!/usr/bin/env python

import argparse
import subprocess
import os

parser = argparse.ArgumentParser(description='Run genetic synthesizer multiple times')
parser.add_argument('genac', help="Path to genac")
parser.add_argument('outputdir', help="Output directory")

coeffs = [
    # numerator  denominator
    ("001",      "111"),
    ("010",      "111"),
    ("100",      "111"),
    ("101",      "111"),
    ("111",      "111"),
    ( "11",       "11")
]

actives = [
    "OPAMP",
    "OTRA",
    "CFOA",
    "CCII",
]

capacitors = [1, 2, 3]

resistors = [2, 3, 4, 5]

population_size = 20
ngenerations = 500
selection_type = "RANK"
crossover_type = "MULTIGENE"
crossover_probability = 0.9


args = parser.parse_args()

for (numerator, denominator) in coeffs:
    for capacitor in capacitors:
        if len(denominator) - 1 > capacitor:
            continue
        for resistor in resistors:
            for active in actives:

                outputdir = os.path.join(
                    args.outputdir,
                    "{active}_{numerator}_{denominator}_{capacitor}C_{resistor}R".format(
                    active=active,
                    numerator=numerator,
                    denominator=denominator,
                    capacitor=capacitor,
                    resistor=resistor
                ))

                subprocess.run(
                    "{genac} genetic-synthesize \
                    --numerator {numerator} \
                    --denominator {denominator} \
                    --ncapacitors {capacitor} \
                    --nresistors {resistor} \
                    --active-element {active} \
                    --population-size {population_size} \
                    --ngenerations {ngenerations} \
                    --selection-type {selection_type} \
                    --crossover-type {crossover_type} \
                    --crossover-probability {crossover_probability} \
                    --outputdir {outputdir}".format(
                        genac=args.genac,
                        numerator=numerator,
                        denominator=denominator,
                        capacitor=capacitor,
                        resistor=resistor,
                        active=active,
                        population_size=population_size,
                        ngenerations=ngenerations,
                        selection_type=selection_type,
                        crossover_type=crossover_type,
                        crossover_probability=crossover_probability,
                        outputdir=outputdir
                    ),
                    shell=True,
                    check=True,
                    stdout=subprocess.PIPE,
                    universal_newlines=True
                )


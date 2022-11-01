#set terminal animate gif crop delay 100 optimize
set term gif animate delay 10
set output 'animation.gif'

set yrange [-2:2]
set xrange [1:1000]

do for [i=2:100]{
	plot 'animation.dat' index i:i:2 with lines notitle
}

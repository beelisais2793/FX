#!/usr/bin/env perl
use warnings;
use strict;

$_ = $^O;

if (/linux/i) {
  qx(echo -std=c++11 > flags.txt);
}
else {
  qx(echo -std=c++11 -stdlib=libc++ > flags.txt);
}

-e "../Cuttlebone" or die $!;
-e "../../cuttlebone" or die $!;
-e "../../AlloSystem" or die $!;
-e "../../AlloSystem/alloutil" or die $!;

chdir "../../AlloSystem/alloutil";
qx(ln -s ../../cuttlebone/Cuttlebone .);


#!/usr/bin/env sh

DIR="$( cd "$(dirname "$0")" ; pwd -P )"
cd "$DIR"

echo "Downloading..."

wget --no-check-certificate http://www.verlab.dcc.ufmg.br/semantic-hyperlapse/data/coolnet/coolnet_pretrained.caffemodel
wget --no-check-certificate http://www.verlab.dcc.ufmg.br/semantic-hyperlapse/data/coolnet/train_mean.binaryproto

mv coolnet_pretrained.caffemodel ../model
mv train_mean.binaryproto ../model

echo "Done."

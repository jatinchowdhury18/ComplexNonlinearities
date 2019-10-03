#!/bin/sh

for f in */*.ipynb
do 
    echo "Copying $f"
    scp $f jatin@ccrma-gate.stanford.edu:~/Library/Web/ComplexNonlinearities/
done

echo "SSH-ing to server..."
ssh jatin@ccrma-gate.stanford.edu <<'ENDSSH'

cd Library/Web/ComplexNonlinearities

for nb in *.ipynb
do
    echo "Deploying $nb"
    jupyter nbconvert $nb
    rm $nb
done

ENDSSH

# echo "Deploy script completed."

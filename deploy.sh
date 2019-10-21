#!/bin/sh

# Copy notebooks
for f in */*.ipynb
do 
    echo "Copying $f"
    scp $f jatin@ccrma-gate.stanford.edu:~/Library/Web/ComplexNonlinearities/
done

# Copy papers
for f in */Paper/*.pdf
do
    echo "Copying $f"
    scp $f jatin@ccrma-gate.stanford.edu:~/Library/Web/ComplexNonlinearities/
done

# Deploy notebooks as html
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

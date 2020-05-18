#!/bin/sh

# Copy notebooks
for f in */*.ipynb
do 
    if [[ $f == *"AlphaTransform.ipynb" ]]; then
        continue
    fi
    echo "Copying $f"
    # scp $f jatin@ccrma-gate.stanford.edu:~/Library/Web/ComplexNonlinearities/
done
exit

# Copy papers
for f in */Paper/*.pdf Paper/*.pdf
do
    echo "Copying $f"
    scp $f jatin@ccrma-gate.stanford.edu:~/Library/Web/papers/
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

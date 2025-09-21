docker run --privileged --rm -v "$PWD":/work -w /work \
    cgr.dev/chainguard/melange build packaging/melange/melange.yaml --arch x86_64
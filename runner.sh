#!/usr/bin/env bash

# Just run the pass.
opt-18 -load-pass-plugin=build/lib/libMutate.so -passes=mutate -S dbg-input.ll -o dbg-out.ll 

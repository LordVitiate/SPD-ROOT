 ---------------------------------------------- 
 Example of K0 finder using KFParticle package
 ---------------------------------------------- 

 1. generation of Minimum Bias event plus K0 particle 
 
    a) SimuPy6_K0.C - Pythia6    
    b) SimuPy8_K0.C - Pythia8

 2. run reconstruction 
 
    a) RecoEventFull_K0.C   - full chain
    b) RecoEventBase_K0.C   - base reconstruction, 
       RecoEventVertex_K0.C - find and fit vertices separately
    
 3. run K0 finder (K0decay.C)
 
    make plots:
    -----------
     - invariant V0 mass plots with different cuts
     - Armenteros Podolanski plot  

 4. for quick check algorithm for V0 finding you can use
    "in flight" V0 finder (run findDecayV0.C), which
    uses SpdVertexCombiFinder() and KFParticle: 

    make plots:
    -----------
     - invariant V0 mass plots with different cuts
     - Armenteros Podolanski plot

  

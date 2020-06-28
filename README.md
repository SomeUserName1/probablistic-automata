# Repository containing materials for the Master's project & thesis
 
see ![Proposal](doc/proposal/proposal.pdf) and  ![Presentation](doc/proposal/presentation.pdf)  
For requirements and software design see the appendix of the ![report draft](doc/project_report/0-main.pdf)  

## TODOs  
- [x] Implement sparse matrix internals of the algorithm in addition to dense matrices  
- [x] Presentation on WA paper & implementation  
- [x] Implement Lifting for WAs/Benchmarks  
- [x] parser for rewriting systems
- [ ] Parser for ODE
- [ ] higher order ODE to system of first order ODEs  
- [ ] ODE to rewriting system conversion
- [ ] Implementation of rewriting system algorithm   
- [ ] Presentation on PNAS paper & implementation  
- [ ] Compare minimizations, in terms of speed, memory & coherrence   

Optional:  
- WA:  
- [x] Normalize by K * n^2 to deminish numerical error but keeping the prob, bounds of Schwarz-Zippel
- [x] Kahan sum for rho vectors to improve numerical stability    
- [ ] Check if Q matrix (i.e. product of householder reflectors) helps with basis   
    (with F forward basis: QF = F' such that F' triangular, thus F'mu more sparse) 

- ERODE:
- [ ] When really fast: impl. DE with z3    

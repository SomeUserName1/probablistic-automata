# Repository containing materials for the Master's project & thesis
 
see ![Proposal](doc/proposal/proposal.pdf) and  ![Presentation](doc/proposal/presentation.pdf)  
For requirements and software design see the appendix of the ![report draft](doc/project_report/0-main.pdf)  

## TODOs  

- [x] parser for rewriting systems
- [ ] Implementation of rewriting system algorithm (WIP)  
- [ ] Parser for ODE
- [ ] higher order ODE to system of first order ODEs  
- [ ] ODE to rewriting system conversion
   
- [ ] Presentation on PNAS paper & implementation  
- [ ] Compare minimizations, in terms of speed, memory & coherrence   

Optional:  
- WA:  
- [x] Normalize by K * n^2 to deminish numerical error but keeping the prob, bounds of Schwarz-Zippel
- [x] Kahan sum for rho vectors to improve numerical stability    
- [ ] Check if Q matrix (i.e. product of householder reflectors) helps with basis   
    (with F forward basis: QF = F' such that F' triangular, thus F'mu more sparse) 

- DE/RS:
- [ ] impl support non-polynomial functions using power & taylor series expansion, symmetries/periodicity, laurent polynomials, ...  
(possible extension for thesis in terms of *exact* transformation to RS)
- [ ] When really fast: impl. DE version with z3    

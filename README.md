# Repository containing materials for the Master's project & thesis

### Topics:  
1. Multi-layered Networks  
    Possible goals:  
    1. Project: Apply framework to sample problems, see what is out there and how it is to be modeled with the given tools  
    2. Thesis: Theoretic framework for assigning dynamics to MLNs and elaborate on the properties (expressiveness, equivalence to other approaches, compositionality, ...)  
2. Missing piece in relationship between ODEs and stochastic semantics  
    Possible goal:  
    1. Project: Maybe validate equivalence by computation (Thesis result'd be fixed before the project?)  
    2. Thesis: Elaborate and proof the nature of the relationship   
3. Minimisation of MC  
    Possible goals:  
    1. Implement minimisation algorithm  
    2. extend to countably infinite state scenario (?)  


### Organisation  
- Attend group meetings on Friday, 10.30 am  
- Timeline & 2 page Proposal before project registration  

## TODOs  
- [x] Quick Recap PMFCS course  
- [x] Dig into the topics  
- [x] Formulate and list questions  
- [-] Decide for something  
- [] Start the Proposal draft  
- [] Check neccessary forms  
- [] In-depth recap of PMFCS including all course exercises  

#### Questions  
- Is a combination of the topics possible? E.g. How do DAE/ODE related to different PA models and and the application of dynamic processes to MLNs with respect to optimal traits (executability, succintness, expressiveness)    
- Would a neuro-oriented example be suitable for the MLNs (layers imposed by temporal ordering, spatial scale, histology)  
- Do PA capture all possible stochastic semantics? Do ODEs/DAEs/SDEs capture all possible dynamics?  
- Are the goals filled in (and not provided by Tanja) above suitable? what would better ones be?  
- What other relevant stochastic processes exist besides the Markovian and the Wiener process (brownian motion)?  
- Are all Systems reducible to fullfill the markov property if one has an infinite mount of memory?  
- Can non-linear systems be modelled by ODEs or does one need DAEs? When are ODEs insufficient and one needs DAEs? When is the determinant of the Jacobi-M. 0 and when is it not (intuitively)? Is a DAE a PDE where at least one variable's derivative is not used/free?  
- When using Markov chains to model CRN dynamics one ends up with an infinite state space. In non-stochastic automata one would switch from FAs to PDAs. Why still using the MC model with states instead of memory configurations?  
- Is PA minimisation feasible with an infinite state space?  
- Missing piece in MC aggregation/ fragments?  


#### Mail by Tanja, March 31th, 2020  
Hi Fabian,

thanks for the time and interest. Here is the summary of the topics we discussed today:


1. Minimisation for probabilistic models of computation.
https://arxiv.org/pdf/1404.6673 and references [24], [26] therein (do not get upset if this looks too hard at the moment).
Possible goal for Master project: implement minimisation algorithms for PA and WA on a suite of examples.

2. Tropical reductions.
https://arxiv.org/pdf/1303.3963.pdf and my paper attached. 
Possible goal: search for tropical regions with SAT solvers and make applicable to a suite of benchmarks of multi-scale biochemical reaction networks.

3. Multi-layered networks.
Stefano will send you one nice review paper. Attached one paper discussing formalisms: https://www.researchgate.net/profile/Verena_Wolf/publication/49457408_Formalisms_for_Specifying_Markovian_Population_Models/links/5448a4fb0cf22b3c14e3144e/Formalisms-for-Specifying-Markovian-Population-Models.pdf 
Possible goal: assign dynamics to MLNs and explore its properties as a syntax (e.g. compare to reaction network formalism). 


4. Deep reductions for stochastic chemical reaction networks.
Here the recent paper and Denis' github page related to the project:
https://arxiv.org/pdf/2002.01889.pdf
https://github.com/dennerepin/StochNetV2 
Possible goal: enable scalable multi-scale simulation (e.g. from molecules to collectives)/study robustness.
Or: employ Bayesian inference to find parameters of the reaction network model from time-series data (this is a Covid question :p).

5. Finally, maybe you are interested in this - there is a missing piece of work here related to the relationship between ODE and stochastic semantics. A gentle introduction to the topic- by example - is this book chapter: 
https://arxiv.org/pdf/1812.09774 


Please let us know if questions. Also, please write down what you think about each of the topics before we meet next time. 

Cheers, 
  Tanja


#### Mail to Stefano & Tanja, April 9th, 2020  
Hi everybody,  

just wanted to ask whether I should participate and ask questions in tomorrows group meeting or rather just attend and ask sometimes else.  
On the topics:  
I ruled out the tropical abstractions and deep learning related stuff. No special reason here, the other topics seem to be more suitable.  

Regarding the minimization topic:  
The algorithms in the paper iterated over the state space. But as seen in the example in last semesters lecture, the state space for CRNs are ultimately infinite, so the minimization algorithms are not directly applicable to CRNs, right?  
Would the topic then be on an arbitrary example or rather on an extension of the algorithms to more generative/compact formalisms like stoichiometric equations, etc.?  
Or stated in terms of classic (non-probabilistic automata): If the state space is infinite in an FA, the automaton model is changed from a FA to a PDA or a TM. This seems not to be the case with Markov chains. Why? (Effectively the infinite state space would be then an infinite space of different memory configurations either using a stack or a "tape band")  
What would the thesis topic be here if the project is about plain implementation?  

I didn't complete the Schützenberger paper so far but I will next week.  


Regarding the MLN Topic:  
Would a neuro-oriented example be okay for you?  
The Project's goal would be an application study of the theoretically described traits per possible way of probabilistic dynamic modelling, right?  

Regarding the relation of ODEs and stochastic semantics:  
Is that on the different number of fragments in the ODE and MC setting or rather on the nature of the ODEs of the states of the MC vs. the nature of the ODEs describing the dynamics?  
Or to make it more explicit: What is the missing relation that shall be characterized?  


General Questions on topic selection:  
- Is a combination of the topics possible? E.g. How do different PA models (memory & transition function) and stochastic processes relate to ODEs, DAEs & SDEs combined with the application of dynamic stochastic processes to MLNs with respect to optimal traits (executability, succinctness, expressiveness, ...)      
- The goals sometime refer to the project goals and sometimes to a thesis goal, right?  

Probabilistic Modelling questions:  
- Do PA capture all possible stochastic semantics?   
- Are there stochastic processes that an MC can not model but a different PA can (besides non-determinism, i.e. on the stochastic side)?  
- Are all Systems reducible to fulfill the Markov property if one has an infinite amount of memory?  
- The PA types differ in the definition of the transition function e.g. in Sokolova & de Vink the most general automaton model defines the transition function as (with P the powerset, D a distribution, S the states, alpha the transition function/matrix, A the set of actions and U. the disjoint union):  alpha: S --> P(D(P(A x S U. S))). What about Memory as with non-probabilistic automaton models?  
- Are there prob. grammar hierarchies with corresponding prob. automata like the Chomsky hierarchy in the non-probabilistic setting (I only know the probabilistic Turing Machine model, how does that align with the hierarchy presented by Sokolova & de Vink)?  
 
Dynamics Modelling questions:  
Can Differential-Algebraic Equations model all dynamic processes (assuming Stochastics may be introduced using SDEs)?  
- Can non-linear systems be modelled by ODEs or does one need DAEs?  
- When are ODEs insufficient and one needs DAEs to model non-linear dynamics?  
- Statetd differently: When is the determinant of the Jacobi-M. 0 and when is it not (intuitively)?  
- Is a DAE a PDE where at least one variable's derivative is not used/free?  

I'm of course not expecting answers to all of those questions, but rather directions and further resources to the ones you consider important.  

How are the group meetings held? And how can I join them?  

@Stefano: I added you to a GitHub Repo where I'd work on the project and thesis (literature, tex files, TODOs, source code and meeting summaries)  
@Tanja Shall I also add you to the repository? In case I'd need your GitHub user name  

Cheers,  
Fabian  

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

#### Invitation to group meeting Fr, 10th April
Feel free to join today's meeting, it will be useful so you see what we do and discuss normally and I will make a brief presentation.

We are trying out different platforms throughout the weeks in order to find one that we will use permanently. Today Tanja decided to go with Zoom, the meeting starts at 10.30 but first we need to discuss few administrative and burocratic stuff. Realisticaly the presentation will start around 11. I will send you the links.

I saw the email on the phone about the github. Unfortunately, the account I had was disconnected from the computer and I had to restore it today (I formatted the PC recently) and the invitation expired. So, if you can, please send me the invitation on github again.



#### Stefanos presentation & QA to the questions by mail via Skype, 10.04.2020  
Papers:  
[1] “On the dynamical interplay between awareness and epidemic spreading in multiplex networks”, C. Granell, S. Gomez, A. Arenas.  
[2] “Competing memes propagation on Networks: A case study on composite networks”, X. Wei, N. Valler, B. Parkash,I. Neamitu, M.
Faloutsus, C. Faloutsus.  
[3] “Reducing spreading processes on Networks to Markov Population Models”, G. Grossmann, L. Bortolussi  

Brief summary:  
- Modelling in MLN: Microscopic MC vs. Mean field approaches  
- Alex Arenas as good reference in the MLN community, Manilo De Domenico as reference for multi-dimensional data on biological neuron networks    
- Lumpability & minimisation probabily better topic than general formalisms on network topology and modelling  
- Check papers from survey for lumpability/minimisation/model reduction methods
- ODEs can model all non-linear dynamics, DAEs introduce constraints (e.g. Ohms and Kirchoffs law in EE, preservation laws)  
- PDEs reducible to DAEs up to a certain index, PDAEs, BDF as numerical solver for all sorts of DE  
- Book: Stochastic chemical reactions  
- MC state space for CRNs only theoretically unlimited, in practice one can't have unlimited molecules  
- ODE and stochastic procs: stochastic model should converge to ODE model in the limit (law of large numbers?/ Monte Carlo?)  


#### Answer by Tanja, Saturday, 11th April
Hi Fabian,

here some answers:


> just wanted to ask whether I should participate and ask questions in tomorrows group meeting or rather just attend and ask sometimes else.
>
> On the topics:
> I ruled out the tropical abstractions and deep learning related stuff. No special reason here, the other topics seem to be more suitable.
>
> Regarding the minimization topic:
> The algorithms in the paper iterated over the state space. But as seen in the example in last semesters lecture, the state space for CRNs are ultimately infinite, so the minimization algorithms are not directly applicable to CRNs, right?

True, in general reduction algorithms may work on infinite state spaces, but minimisations no.

> Would the topic then be on an arbitrary example or rather on an extension of the algorithms to more generative/compact formalisms like stoichiometric equations, etc.?

It would be on PA's wrt. trace semantics and different relaxations of trace semantics.

> Or stated in terms of classic (non-probabilistic automata): If the state space is infinite in an FA, the automaton model is changed from a FA to a PDA or a TM. This seems not to be the case with Markov chains. Why? (Effectively the infinite state space would be then an infinite space of different memory configurations either using a stack or a "tape band")
> What would the thesis topic be here if the project is about plain implementation?
>
> I didn't complete the Schützenberger paper so far but I will next week.

> Regarding the MLN Topic:
> Would a neuro-oriented example be okay for you?
> The Project's goal would be an application study of the theoretically described traits per possible way of probabilistic dynamic modelling, right?
>
> Regarding the relation of ODEs and stochastic semantics:
> Is that on the different number of fragments in the ODE and MC setting or rather on the nature of the ODEs of the states of the MC vs. the nature of the ODEs describing the dynamics?
> Or to make it more explicit: What is the missing relation that shall be characterized?
>
>
> General Questions on topic selection:
> - Is a combination of the topics possible? E.g. How do different PA models (memory & transition function) and stochastic processes relate to ODEs, DAEs & SDEs combined with the application of dynamic stochastic processes to MLNs with respect to optimal traits (executability, succinctness, expressiveness, ...)
> - The goals sometime refer to the project goals and sometimes to a thesis goal, right?
>
> Probabilistic Modelling questions:
> - Do PA capture all possible stochastic semantics?

Depends - probably Rabin PA's or Input/Output PA's capture many things involving internal/external non-determinism+uncertainty. But maybe there is need for notions beyond existing objects.
That is in fact one possible topic to consider - explore which objects you need if you compose PA's (please check the draft called 'draft JanAna.pdf' and see if you understand - basically if you want to compose two non-det. Rabin PA's so to get a union of their languages in the composition, you would need a different automaton performing some convex combinations).

> - Are there stochastic processes that an MC can not model but a different PA can (besides non-determinism, i.e. on the stochastic side)?

with PA's you can assign labels to states or transitions so you can reason more abstractly about the executions (e.g. with logics); also, you typically look at the language semantics and with MC's traditionally you take the transition matrix and analyse numerically. But in general you keep MC notion as long as it is expressive enough for your purpose.
> - Are all Systems reducible to fulfill the Markov property if one has an infinite amount of memory?
No. There are systems with infinite memory (in MDPs for randomised strategies you may need this), but I have not worked with those.

> - The PA types differ in the definition of the transition function e.g. in Sokolova & de Vink the most general automaton model defines the transition function as (with P the powerset, D a distribution, S the states, alpha the transition function/matrix, A the set of actions and U. the disjoint union): alpha: S --> P(D(P(A x S U. S))). What about Memory as with non-probabilistic automaton models?
> - Are there prob. grammar hierarchies with corresponding prob. automata like the Chomsky hierarchy in the non-probabilistic setting (I only know the probabilistic Turing Machine model, how does that align with the hierarchy presented by Sokolova & de Vink)?
>
> Dynamics Modelling questions:
> Can Differential-Algebraic Equations model all dynamic processes (assuming Stochastics may be introduced using SDEs)?
> - Can non-linear systems be modelled by ODEs or does one need DAEs?
> - When are ODEs insufficient and one needs DAEs to model non-linear dynamics?
> - Statetd differently: When is the determinant of the Jacobi-M. 0 and when is it not (intuitively)?
> - Is a DAE a PDE where at least one variable's derivative is not used/free?

A useful reference: https://arxiv.org/pdf/1607.02966.pdf

>
> I'm of course not expecting answers to all of those questions, but rather directions and further resources to the ones you consider important.
>
> How are the group meetings held? And how can I join them?
>
> @Stefano: I added you to a GitHub Repo where I'd work on the project and thesis (literature, tex files, TODOs, source code and meeting summaries)
> @Tanja Shall I also add you to the repository? In case I'd need your GitHub user name

Apart from the literature I already sent you, it is worth looking to the literature on population automata/automata networks and languages thereof.

I will soon compile the list of papers for the probabilistic seminar this year and will send you, so this may be of interest, too.

my name on github is tatjanapetrov

Cheers!
Tanja

#### Addition on Bayesian Model checking by Tanja, Saturday 11th April
Since this was not in the list, and I saw you did last year a seminar on numerical vs. stat. MC, here is one paper that is now actual - Bayesian MC. Please let me know how this looks like to you (i.e. if you understand the ideas):

https://www.cs.ox.ac.uk/people/alessandro.abate/publications/bcMWA19.pdf

This could be a very well fitting topic for our work on bees collective behaviour project, where we have data available. 

Best, 
 Tanja


#### Topic fixation proposal by Stafano, Monday, 13th April
Dear Fabian,

as we agreed in our chat, after discussions between Prof. Petrov and me, we are thinking of the following possibility for a project and master thesis.

The project will involve some more hands-on, implementation tasks on various literature notions along with some complemental reading. More specifically, the main goals of the project would be the following:
1. Implement the polynomial minimisation of weighted automata
2. Implement different notions of Differential Equivalence (forward and backward) on weighted automata [there is a state of the art reducer for Chemical Reaction Networks, called ERODE, it will be useful to learn how it works and use it for reductions]
3. Compare the methods and the results: do both reduction output the same minimal WA?
I will provide some literature in the git-hub repo about (1.) and (2.). During the project it would be nice to implement the first two parts, to think about a suitable way of producing some benchmarks WA that can be used throughout the whole project and to become familiar with the notions of Differential Equivalence and Weighted Automata.

Then, ideally, after the project we can proceed to explore novel directions for the thesis. For instance, we can extend such reductions to weighted networks, multilayer networks or Probabilistc Automata. Which will be the best direction to follow shall be decided once the first part of the project is underway.

Let us know if you would be content with such a plan so we can proceed and you can start writing your proposal.

Best regards,
have a nice day,
Stefano.

#### Answer to topic fixation by author, Tuesday, 14th April
Dear Stefano,

looks good to me, thanks for the guidance!
I start writing the proposal, it should be finished within the next days.

Regards,
Fabian

#### Answer to Tanjas answers by author, Tuesday, 14th April
Hi Tanja,

thanks for the answers and your time!
The paper is quite useful wrt. the topic that Stefano sent me.
I added you to the GitHub project.

Cheers,
Fabian

#### Project and potential presentation Friday, by Stefano, April 17th 2020
Dear Fabian,

I hope this message finds you well.
We were thinking that it might be a good idea if in the next unit meeting (Friday 24th of April at 10:30) you could give a presentation about your project. We don't expect many new contents in the presentation, ideally the presentation would be a 15mins high-level explanation on the project: what are the goals, what is the relevant literature, motivations and timeline. Basically, it would be a slideshow of the project proposal.
It will be good for everybody to at least have an overview of what will be done in the following months.

Moreover, we were thinking to have something similar within the seminar course of advanced topics for probabilistic modelling, but there is still time to think about this.

Let us know if this is feasible,
have a nice weekend,
Stefano.

#### Vodafone cuts off internet connection, by Fabian, April 17th 2020
Hey Stefano, 

yeah this sounds reasonable. 
Sorry for the silence this week. 
Since Tuesday I have no proper Internet connection (started with 200kB/s max speed, stopped working entirely yesterday after a Vodafone technician fiddled with the house switch...). On Monday another technician appointment is scheduled and I hope this s--- will be resolved. 

If the connection is still down after that I'm going to get a prepaid SIM and connect via Hotspot so that the presentation/meeting will not get interrupted. 
I try to send you the slides latest on Wednesday evening next week (via email & pdf). 

Will the presentation be on Zoom like 2 weeks ago? 

You too & thanks for the guidance, 
Fabian

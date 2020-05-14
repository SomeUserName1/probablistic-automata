function WA_kiefer
  RandomBasisSchuetzenbergerConstruction_AlphaAdded();
end

function [] = RandomBasisSchuetzenbergerConstruction_AlphaAdded()
  pkg load statistics;
  pkg load symbolic;
  
  rng(1);

  alpha = [1 0 0 0];
  mu_a   = [0 1 1 0 ; 0 0 0 0 ; 0 0 0 0 ; 0 0 0 0 ];
  mu_b   = [0 0 0 0 ; 0 0 0 1 ; 0 0 0 1 ; 0 0 0 0 ]; 
  eta   = [0;0;0;1];

  K = 3; 

  %% Forward reduction
  %%r1 = unidrnd( K*4 , 2 , 4 );
  %%r2 = unidrnd( K*4 , 2 , 4 );
  %%r3 = unidrnd( K*4 , 2 , 4 );
  %%r4 = unidrnd( K*4 , 2 , 4 );
  % example from report
  r1 = [9 5 5 7; 6 11 2 1];
  r2 = [2 3 1 2; 12 3 9 4];
  r3 = [2 7 9 10; 1 11 2 6];
  r4 = [4 5 2 10; 5 9 5 5];
  
  
  v1 = r1(1,1) * [0 1 1 0] + r1(1,1)*r1(2,2) * [0 0 0 2] + [1 0 0 0];
  v2 = r2(1,1) * [0 1 1 0] + r2(1,1)*r2(2,2) * [0 0 0 2] + [1 0 0 0];
  v3 = r3(1,1) * [0 1 1 0] + r3(1,1)*r3(2,2) * [0 0 0 2] + [1 0 0 0];
  v4 = r4(1,1) * [0 1 1 0] + r4(1,1)*r4(2,2) * [0 0 0 2] + [1 0 0 0];

  F = [v1; v2; v3; v4];
  F = F(1:rank( F ), 1:4);

  %F_mu_a = F*mu_a;
  %F_mu_b = F*mu_b;
  %mu_a_arrow = sym('a%d%d', [3 3]);
  %mu_b_arrow = sym('b%d%d', [3 3]);
  %mu_a_arrow_F = mu_a_arrow*F;
  %mu_b_arrow_F = mu_b_arrow*F;
  
  % Alternative: Use pseudo inverse 
  mu_a_arrow1 = F*mu_a*pinv(F);
  mu_b_arrow1 = F*mu_b*pinv(F);


  eta_arrow       = F*eta;
  alpha_arrow     = zeros(1, rank(F));
  alpha_arrow(1)  = 1
  
  mu_a_arrow1
  mu_b_arrow1
  eta_arrow
  alpha_arrow

  disp('Word "b" before minimization');
  alpha*mu_b*eta
  disp('Word "b" after minimization');
  alpha_arrow*mu_b_arrow1*eta_arrow
  
  disp('Word "abbb" before minimization');
  alpha*mu_a*mu_b*mu_b*mu_b*eta
  disp('Word "abbb" before minimization');
  alpha_arrow*mu_a_arrow1*mu_b_arrow1*mu_b_arrow1*eta_arrow
  
end



% octave doesnt have a rng()/seeding function by now.
%function rng(x)
%  randn("seed",x)
%  rand("seed",x)
%end

%function [] = RandomBasisSchuetzenbergerConstruction_AlphaStacked()
%  pkg load statistics;
%  pkg load symbolic;
%  
%  rng(1);

%  alpha = [1 0 0 0];
%  mu_a   = [0 1 1 0 ; 0 0 0 0 ; 0 0 0 0 ; 0 0 0 0 ];
%  mu_b   = [0 0 0 0 ; 0 0 0 1 ; 0 0 0 1 ; 0 0 0 0 ]; 
%  eta   = [0;0;0;1];

%  K = 3; 

  %% Forward reduction
  %%r1 = unidrnd( K*4 , 2 , 4 );
  %%r2 = unidrnd( K*4 , 2 , 4 );
  %%r3 = unidrnd( K*4 , 2 , 4 );
  %%r4 = unidrnd( K*4 , 2 , 4 );
  % example from report
%  r1 = [9 5 5 7; 6 11 2 1];
%  r2 = [2 3 1 2; 12 3 9 4];
%  r3 = [2 7 9 10; 1 11 2 6];
%  r4 = [4 5 2 10; 5 9 5 5];
  
  
%  v1 = r1(1,1) * [0 1 1 0] + r1(1,1)*r1(2,2) * [0 0 0 2];
%  v2 = r2(1,1) * [0 1 1 0] + r2(1,1)*r2(2,2) * [0 0 0 2];
%  v3 = r3(1,1) * [0 1 1 0] + r3(1,1)*r3(2,2) * [0 0 0 2];
%  v4 = r4(1,1) * [0 1 1 0] + r4(1,1)*r4(2,2) * [0 0 0 2];

%  F = [alpha; v1; v2; v3; v4];
%  F = F(1:rank( F ), 1:4);

  %F_mu_a = F*mu_a;
  %F_mu_b = F*mu_b;
  %mu_a_arrow = sym('a%d%d', [3 3]);
  %mu_b_arrow = sym('b%d%d', [3 3]);
  %mu_a_arrow_F = mu_a_arrow*F;
  %mu_b_arrow_F = mu_b_arrow*F;
  
  % Alternative: Use pseudo inverse 
%  mu_a_arrow1 = F*mu_a*pinv(F);
%  mu_b_arrow1 = F*mu_b*pinv(F);


%  eta_arrow       = F*eta;
%  alpha_arrow     = zeros(1, rank(F));
%  alpha_arrow(1)  = 1
  
%  mu_a_arrow1
%  mu_b_arrow1
%  eta_arrow
%  alpha_arrow
  
%  disp('Word "b" before minimization');
%  alpha*mu_b*eta
%  disp('Word "b" after minimization');
%  alpha_arrow*mu_b_arrow1*eta_arrow
  
%  disp('Word "abbb" before minimization');
%  alpha*mu_a*mu_b*mu_b*mu_b*eta
%  disp('Word "abbb" before minimization');
%  alpha_arrow*mu_a_arrow1*mu_b_arrow1*mu_b_arrow1*eta_arrow
%end

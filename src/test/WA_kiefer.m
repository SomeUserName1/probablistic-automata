function WA_kiefer
  %for reproducible results
  rng(1);
  % Stefanos example
  a = sym('a');
  b = sym('b');
  c = sym('c');
  d = sym('d');
  e = sym('e');
  f = sym('f');
  g = sym('g');
  h = sym('h');
  alpha  = [a b c d];
  mu_a   = [0 1 1 0 ; 0 0 0 0 ; 0 0 0 0 ; 0 0 0 0 ];
  mu_b   = [0 0 0 0 ; 0 0 0 1 ; 0 0 0 1 ; 0 0 0 0 ]; 
  eta    = [e;f;g;h];

  % example random vectors from report
  f1 = [9 5 5 7 ;   6 11 2 1];
  f2 = [2 3 1 2 ;   12 3 9 4];
  f3 = [2 7 9 10;   1 11 2 6];
  f4 = [4 5 2 10;   5 9 5 5 ];
 
  % Backward reduction
  [alpha_arrow, mu_a_arrow, mu_b_arrow, eta_arrow] = ...
      ForwardSchuetzenbergerConstruction(alpha, mu_a, mu_b, eta, ...
        f1,f2, f3, f4);
    
  probe_and_display(alpha_arrow, mu_a_arrow, mu_b_arrow, eta_arrow);
    
   % example random vectors from report
%  b1 = [1 6 2;   4 5 4];
%  b2 = [8 1 8;   7 8 2];
%  b3 = [1 6 5;   9 2 1];
    
  % Forward reduction
%   [alpha_arrow2, mu_a_arrow2, mu_b_arrow2, eta_arrow2] = ...
%      BackwardSchuetzenbergerConstruction(alpha_arrow, mu_a_arrow, ...
%        mu_b_arrow, eta_arrow, b1, b2, b3);  
%    
%  probe_and_display(alpha_arrow2, mu_a_arrow2, mu_b_arrow2, eta_arrow2);
end

function [alpha_arrow, mu_a_arrow, mu_b_arrow, eta_arrow] = ...
    ForwardSchuetzenbergerConstruction(alpha, mu_a, mu_b, eta, ...
        i1, i2, i3, i4)
  % If random vectors are passed then use them, else generate some
  switch nargin
      case 8
          r1 = i1; r2 = i2; r3 = i3; r4 = i4;
      case 4
          K = 3; 
          r1 = unidrnd( K*4 , 2 , 4 );
          r2 = unidrnd( K*4 , 2 , 4 );
          r3 = unidrnd( K*4 , 2 , 4 );
          r4 = unidrnd( K*4 , 2 , 4 );
      otherwise
          error("Wrong number of arguments")
  end
  
  % calculate the forward version of v_i = rho(r^i)
  a = sym('a');
  v1 = r1(1,1) * [0 a a 0] + r1(1,1)*r1(2,2) * [0 0 0 2*a];
  v2 = r2(1,1) * [0 a a 0] + r2(1,1)*r2(2,2) * [0 0 0 2*a];
  v3 = r3(1,1) * [0 a a 0] + r3(1,1)*r3(2,2) * [0 0 0 2*a];
  v4 = r4(1,1) * [0 a a 0] + r4(1,1)*r4(2,2) * [0 0 0 2*a];
  
  % Stack alpha as first row vector onto the matrix
  F = [alpha; v1; v2; v3; v4];
  % extract the maximally linearly independent subset
  F = F(1:rank( F ), 1:4);
  disp(F);
  % Calculate the new transition matricies  
  mu_a_arrow = F*mu_a/F;
  mu_b_arrow = F*mu_b/F;

  
  % Calculate the new alpha and eta
  eta_arrow       = F*eta;
  alpha_arrow     = zeros(1, rank(F));
  alpha_arrow(1)  = 1;
end


function [alpha_arrow, mu_a_arrow, mu_b_arrow, eta_arrow] = ...
    BackwardSchuetzenbergerConstruction(alpha, mu_a, mu_b, eta, ...
        i1, i2, i3)
  % If random vectors are passed then use them, else generate some
  switch nargin
      case 7
          r1 = i1; r2 = i2; r3 = i3;
      case 4
          K = 3; 
          r1 = unidrnd( K*4 , 2 , 3 );
          r2 = unidrnd( K*4 , 2 , 3 );
          r3 = unidrnd( K*4 , 2 , 3 );
      otherwise
          error("Wrong number of arguments")
  end
  % calculate the backwards version of v_i = rho(r^i)     
  v1 = r1(2,1) * [0 18 4] + r1(1,1)*r1(2,2) * [2 0 0];
  v2 = r2(2,1) * [0 18 4] + r2(1,1)*r2(2,2) * [2 0 0];
  v3 = r3(2,1) * [0 18 4] + r3(1,1)*r3(2,2) * [2 0 0];
  
  % Stack eta as first column vector of the matrix;; There is probabily a
  % matlab function doing this, but i couldn't find it
  B = [eta(1) v1(1) v2(1) v3(1); 
       eta(2) v1(2) v2(2) v3(2);
       eta(3) v1(3) v2(3) v3(3);];
  B = B(1:3, 1:rank( B ));
  % calculate the backward basis; matrix "division" is faster than
  % pseudoinverse. For impl. details: 
  % https://de.mathworks.com/help/releases/R2020a/matlab/ref/mldivide.html
  mu_a_arrow = B\mu_a*B;
  mu_b_arrow = B\mu_b*B;

  alpha_arrow       = alpha*B;
  eta_arrow     = zeros(rank(B), 1);
  eta_arrow(1)  = 1;
end

function [] = probe_and_display(alpha, mu_a, mu_b, eta)
disp("alpha "); disp(alpha);
    disp("mu a ");  disp(mu_a);
    disp("mu b ");  disp(mu_b);
    disp("eta ");   disp(eta);
    disp("______________________________________")
    mu_a_eta = alpha*mu_a*eta;
    mu_b_eta = alpha*mu_b*eta;
    mu_a_2 = alpha*mu_a*mu_a*eta;
    mu_b_2 = alpha*mu_b*mu_b*eta;
    mu_a_b = alpha*mu_a*mu_b*eta;
    mu_b_a = alpha*mu_b*mu_a*eta;
    mu_a_a_b = alpha*mu_a*mu_a*mu_b*eta;
    mu_b_a_b = alpha*mu_b*mu_b*mu_a*eta;
    
    disp("alpha mu a eta"); disp(mu_a_eta);
    disp("alpha mu b eta"); disp(mu_b_eta);
    disp("alpha mu a^2 eta"); disp(mu_a_2);
    disp("alpha mu b^2 eta"); disp(mu_b_2);
    disp("alpha mu a mu b eta"); disp(mu_a_b);
    disp("alpha mu b mu a eta"); disp(mu_b_a);
    disp("alpha aab eta"); disp(mu_a_a_b);
    disp("alpha bab eta"); disp(mu_b_a_b);
    
    prev_err = alpha*mu_a*mu_b*mu_b*mu_b*eta;
    disp("alpha abbb eta"); disp(prev_err);
    
    prev_err1 = alpha*mu_a*mu_b*mu_b*mu_b*mu_b*eta;
    disp("alpha abbbb eta"); disp(prev_err1);
    
    prev_err2 = alpha*mu_b*eta;
    disp("alpha b eta"); disp(prev_err2);
    disp("______________________________________")
end

function [equal] = numeric_isequal(mat1, mat2, precision)
    switch nargin
        case 3
            prec = precision;
        case 2
            prec = 1e-10;
        otherwise
            error("Wrong number of arguments")
    end
    assert(isequal(size(mat1), size(mat2)))
    equal = isequal(abs(mat1 - mat2) < prec*ones(size(mat1)), ...
        ones(size(mat1)));
end
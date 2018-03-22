%
% Copyright (c) 2016, Yarpiz (www.yarpiz.com)
% All rights reserved. Please read the "license.txt" for license terms.
%
% Project Code: YTEA101
% Project Title: Particle Swarm Optimization Video Tutorial
% Publisher: Yarpiz (www.yarpiz.com)
% 
% Developer and Instructor: S. Mostapha Kalami Heris (Member of Yarpiz Team)
% 
% Contact Info: sm.kalami@gmail.com, info@yarpiz.com
%

function [SS, SNS, S] = SpeedupOptimizationPSOSetup(Ts, Tns, Sb)

% clc;
% clear;
% close all;

%% Parameters of PSO

params.MaxIt = 30;        % Maximum Number of Iterations
params.nPop = 30;           % Population Size (Swarm Size)
params.w = .99;               % Intertia Coefficient
params.wdamp = .99;        % Damping Ratio of Inertia Coefficient
params.c1 = 2;              % Personal Acceleration Coefficient
params.c2 = 2;              % Social Acceleration Coefficient
params.ShowIterInfo = false; % Flag for Showing Iteration Informatin
params.Ts = Ts;
params.Tns = Tns;
params.Sb = Sb;

%% Problem Definiton

problem.CostFunction = @(x, params) speedups_fitness(x, params);  % Cost Function
problem.nVar = 2;       % Number of Unknown (Decision) Variables
problem.VarMin =  10;  % Lower Bound of Decision Variables
problem.VarMax =  100;   % Upper Bound of Decision Variables

%% Calling PSO

out = SpeedupOptimizationPSO(problem, params);

%rng default
%options = optimoptions('particleswarm','SwarmSize', params.nPop,...
%    'MaxIterations', params.MaxIt, 'MaxStallIterations', 8,...
%    'InertiaRange', [0.2 5], 'UseParallel', true, 'Display', 'iter');
            

%BestSol.Position = particleswarm(problem.CostFunction,problem.nVar,...
%                    [0 0], [],options);  

BestSol = out.BestSol;
%BestCosts = out.BestCosts;
%disp([out.BestSol.Position(1) out.BestSol.Position(2)]);


[SS, SNS, Z, S] = SpeedupOptimizationFunc( params.Tns, params.Ts, params.Sb, 100, BestSol.Position(1), BestSol.Position(2), 0);
    

%% Results
%fprintf('\tlambda1 => %f \t lambda2 => %f \t R => %f \t ps => %f\n',...
%    BestSol.Position(1), BestSol.Position(2), BestSol.Position(2)/BestSol.Position(1), Ts/(Ts+Tns));

end


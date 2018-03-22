%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%   This file is part of SemanticFastForward_JVCI.
%
%    SemanticFastForward_JVCI is free software: you can redistribute it and/or modify
%    it under the terms of the GNU General Public License as published by
%    the Free Software Foundation, either version 3 of the License, or
%    (at your option) any later version.
%
%    SemanticFastForward_JVCI is distributed in the hope that it will be useful,
%    but WITHOUT ANY WARRANTY; without even the implied warranty of
%    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%    GNU General Public License for more details.
%
%    You should have received a copy of the GNU General Public License
%    along with SemanticFastForward_JVCI.  If not, see <http://www.gnu.org/licenses/>.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% Class Name: SemanticSequenceLibrary
% 
% This class is an adaptation of the SequenceLibrary class present
% in the EgoSampling code. It contains configuration details to run  
% the Semantic Fast-Forward code.
%
% $Date: July 26, 2017
% ________________________________________
classdef SemanticSequenceLibrary
    methods(Static)
        
        function [videoFile, startInd, endInd, filename, fps] = GetVideoDetails(video_dir,exp_name)
            switch exp_name
                %% Semantic_Dataset
                case 'Biking_0p'
                     filename = 'Biking_0p.MP4';
                     startInd=1;
                     endInd=17949;
                     fps = 60;
                case 'Biking_25p'
                     filename = 'Biking_25p.MP4';
                     startInd=1;
                     endInd=17071;
                     fps = 30;
                case 'Biking_50p'
                     filename = 'Biking_50p.MP4';
                     startInd=1;
                     endInd=26954;
                     fps = 60;
                case 'Biking_50p_2'
                     filename = 'Biking_50p_2.MP4';
                     startInd=1;
                     endInd=14939;
                     fps = 60;
                case 'Driving_0p'
                     filename = 'Driving_0p.MP4';
                     startInd=1;
                     endInd=9463;
                     fps = 30;
                case 'Driving_25p'
                     filename = 'Driving_25p.MP4';
                     startInd=1;
                     endInd=7989;
                     fps = 30;
                case 'Driving_50p'
                     filename = 'Driving_50p.MP4';
                     startInd=1;
                     endInd=10379;
                     fps = 30;
                case 'Walking_0p'
                     filename = 'Walking_0p.MP4';
                     startInd=1;
                     endInd=8219;
                     fps = 30;
                case 'Walking_25p'
                     filename = 'Walking_25p.MP4';
                     startInd=1;
                     endInd=10982;
                     fps = 30;
                case 'Walking_50p'
                     filename = 'Walking_50p.MP4';
                     startInd=1;
                     endInd=11570;
                     fps = 30;
                case 'Walking_75p'
                    filename = 'Walking_75p.MP4';
                    startInd=1;
                    endInd=15481;
                    fps = 30;
                case 'Walking_75p_2'
                    filename = 'Walking_75p_2.MP4';
                    startInd=1;
                    endInd=7474;
                    fps = 30;
                %% An Example for usage
                case 'Example'
                    filename = 'example.mp4';
                    startInd = 1;
                    endInd   = 884;
                    fps      = 30;
                otherwise
                    error(['ERROR: You have specified a non-existent experiment (' exp_name ')']);
            end
            
            videoFile = fullfile(video_dir,filename);
        end
        
        function [cfg] = GetFFExperimentDetails(video_dir, exp_name, algorithm, required_speedup, skip_video_output)
            
            [videoFile, startInd, endInd, filename, fps] = SemanticSequenceLibrary.GetVideoDetails(video_dir,exp_name);
            
            [~, fname ,~] = fileparts(filename);
            
            framesDumpDir = strrep(fullfile(video_dir,['/dump/' fname]),'\','/');
            framesDumpFormat = strrep(fullfile(framesDumpDir,'/frame_%06d.png'),'\','/');
            
            if ~exist(sprintf(framesDumpFormat,startInd),'file')
                framesDumpFormat = strrep(fullfile(framesDumpDir,'/frame_%05d.png'),'\','/');
            end
            if ~exist(sprintf(framesDumpFormat,startInd),'file')
                framesDumpDir = strrep(fullfile(video_dir,[fname '_frames_undist']),'\','/');
                framesDumpFormat = [framesDumpDir '/frame_%05d.png'];
            end
            if ~exist(sprintf(framesDumpFormat,startInd),'file')
                framesDumpFormat = [framesDumpDir '/frame_%06d.png'];
            end
            
            
            % Generally, this is the config. For certain types of experiments,
            % we have a switch statement below to adjust stuff.
            cfg = ConfigWrapper({'inputVideoFileName',videoFile;...
                'FileName', fname;...
                'ExpName', exp_name;...
                'VelocityTarget','CumulativeMean';...
                'FastForwardSkipRatio', required_speedup;...
                'terminalConnectionDegree', 1;...
                
                'baseDumpFrameFileName',framesDumpFormat;
                'ShakinessCostFunction','FOE';
                'ShakinessTermWeight',10;...
                'ShakinessTermInvalidValue',100000;...
                
                'VelocityTermWeight',10;...
                'AppearanceCostFunction','ColorHistogram';
                'AppearanceTermWeight',2;...
                
                'SemanticCostFunction','Semantic';
                'SemanticTermWeight',1;...
                'SemanticEpsilon', 1;...
                
                'ForwardnessTermWeight',4;...
                'ForwardnessMergeWithShakinessCoef', 0;...
                'ForwardnessCostFunction','None';...
                
                'FrameSelector', 'ShortestPath';
                'UseHigherOrder',false;
                'maxTemporalDist',100;
                'FOE_Reference','Absolute';...
                'startInd',startInd;
                'endInd',endInd;
                'SkipVideoOutput',skip_video_output;
                'CostWeightMethod','Sum';
                
                'SaveFramesWhileDumping', 0;
                'ShowOutputWhileDumping', 0;
                'OutputFOEMovements', 0;
                'OutputFOEPoints', 0;
                'OutputSemanticBoxes', 0;
                'OutputSemanticWeight', 0;
                'OutputInstabilityValues', 0;
                'OutputOriginalTimestamp', 0;
                'OutputOriginalFrameNum', 0;
                'OutputTheoreticalSpeedup', 0;
                
                % Weights from semantic ranges
                'ShakinessTermWeight', [];
                'VelocityTermWeight', [];
                'AppearanceTermWeight', [];
                'SemanticTermWeight', [];
                'ForwardnessTermWeight', [];
                'HighOrderTermWeight', [];
                
                % Semantic ranges
                'SemanticRanges', [];
                'FPS', fps;
                'UseMultipleSemanticThresholds', true;
                
                %SemanticFunction
                'SemanticFunction', 'NormScore';
                
                %Charts
                'PlotCharts', 0;
                'SaveCharts', 0;
                                
                %Optimization params (PSO)                
                %% Problem Definiton
                'PSO.problem.nVar', 8;       % Number of Unknown (Decision) Variables
                'PSO.problem.VarMin', 10;     % Lower Bound of Decision Variables
                'PSO.problem.VarMax', 50;   % Upper Bound of Decision Variables
                
                %% Parameters of PSO                
                'PSO.params.MaxIt', 30;        % Maximum Number of Iterations
                'PSO.params.nPop',  15;           % Population Size (Swarm Size)
                'PSO.params.w', 1;               % Intertia Coefficient
                'PSO.params.wdamp', 0.99;        % Damping Ratio of Inertia Coefficient
                'PSO.params.c1', 2;              % Personal Acceleration Coefficient
                'PSO.params.c2', 2;              % Social Acceleration Coefficient
                'PSO.params.ShowIterInfo', 1; % Flag for Showing Iteration Informatin
                
                });
            
            switch algorithm
                case 'Naive'
                    cfg.set('FrameSelector', 'Naive');
                case 'NaiveSemantic'
                    cfg.set('FrameSelector', 'NaiveSemantic');
                case 'EgoSampling'
                    cfg.set('SemanticCostFunction','None');
                case 'FFSE'
                    cfg.set('FrameSelector', 'ShortestPath');
                otherwise
            end
            
        end
    end        
end
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

classdef SemanticCost < InterframeCost
%% Class that handles the semantic cost
    properties
        SemanticData;
    end
    methods
        function obj = SemanticCost(sd, cfg, SemanticData)
            obj = obj@InterframeCost(sd, cfg);
            obj.SemanticData = SemanticData;
        end
        
        function cost = calculateCostBatchListFrames...
                (obj, ~, ~, frame_indices, startBatch, endBatch)
            
            temporal_dist = obj.cfg.get('maxTemporalDist');
            
            lastFullEstimation = min(endBatch + temporal_dist, obj.cfg.get('endInd'));
            
            obj.SemanticData(startBatch:lastFullEstimation);
            
            semanticFunction = obj.cfg.get('SemanticFunction');
            
            switch semanticFunction
                case 'MaxScore'
                    cost = obj.MaxScore(obj.SemanticData, frame_indices, temporal_dist, lastFullEstimation, startBatch, endBatch);
                case 'NormScore'
                    epsilon = obj.cfg.get('SemanticEpsilon');
                    cost = obj.NormScore(obj.SemanticData, epsilon, frame_indices, temporal_dist, lastFullEstimation, startBatch, endBatch);                    
                otherwise
                    error('Unknown ''SemanticFunction'' value "%s"', semanticFunction);
            end
            
        end
        
        function cost = MaxScore(obj, df_dts, frame_indices, temporal_dist, lastFullEstimation, startBatch, endBatch)
            cost = ones(endBatch-startBatch+1,temporal_dist)*nan;
            
            Semantic_Value = ones(numel(frame_indices), 1);
            
            for i=1:numel(frame_indices)
                Semantic_Value(i) = FrameSemanticValue(df_dts{i});
            end
            
            max_score = max(Semantic_Value);
            for i = 1:numel(frame_indices)
                if mod(i,100) == 0
                    fprintf('%sCalculating semantic cost frame %d (%d/%d for this batch)\n',log_line_prefix,frame_indices(i),i,numel(frame_indices));
                end
                
                last = min (temporal_dist, lastFullEstimation-frame_indices(i));
                for j = 1:last
                    %Calculate the Human Semantic Cost between frames
                    cost(frame_indices(i)-startBatch+1,j) = 2*max_score-(FrameSemanticValue(df_dts{i}) + FrameSemanticValue(df_dts{i+j}));
                end
            end
        end
        
        function cost = NormScore(obj, df_dts, epsilon, frame_indices, temporal_dist, lastFullEstimation, startBatch, endBatch)
            cost = ones(endBatch-startBatch+1,temporal_dist)*nan;
            
            for i = 1:numel(frame_indices)
                if mod(i,100) == 0
                    fprintf('%sCalculating semantic cost frame %d (%d/%d for this batch)\n',log_line_prefix,frame_indices(i),i,numel(frame_indices));
                end
                
                last = min (temporal_dist, lastFullEstimation-frame_indices(i));
                for j = 1:last
                   %Calculate the Semantic Cost between frames (End of Section 3.1.4 in the Journal paper)
                   cost(frame_indices(i)-startBatch+1,j) = 1 ./ (FrameSemanticValue(df_dts{i}) + FrameSemanticValue(df_dts{i+j}) + epsilon);
                end
            end
        end
        
        function [cost] = calculateCostBatch ...
                (obj, ~, ~, startBatch, endBatch)
            
            firstImgNum = startBatch;
            lastImgNum  = endBatch;
            
            frame_indices = firstImgNum : lastImgNum;
            
            cost = obj.calculateCostBatchListFrames([], [], frame_indices, startBatch, endBatch);
            
        end
        
        
    end
end
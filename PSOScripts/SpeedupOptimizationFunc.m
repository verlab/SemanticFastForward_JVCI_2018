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

%% Generates Ss and Sns values (Z is the min value and S is the effective speedup achieved if Ss and Sns were chosen)
function [SS, SNS, Z, S] = SpeedupOptimizationFunc(Tns, Ts, Sb, maxTempDist, lmb1, lmb2, show)

    if nargin < 7
        show = 0;
    end

    [Sns, Ss] = meshgrid(1:1:maxTempDist);
    
    ERR = abs((Tns + Ts)/Sb - Ts./Ss - Tns./Sns);
    OF = ERR + lmb1 * (Sns - Ss) + lmb2 * abs(Ss);   

    semantic_percentage = Ts/(Ts+Tns);% Semantic percentage
    ss_floor = semantic_percentage*Sb;% Semantic floor
    
    OF (Ss > Sns | Sns < Sb | Ss <= ss_floor) = nan;% Space Restrictions!
    
    [m, n] = min(OF);
    [Z, SNS] = min(m);
    SS = n(SNS);
    
    S = (Tns + Ts) / (Ts/SS + Tns/SNS);

    if show
        figure;
        title('Search Space','FontSize', 20); 
        xlabel('Non Semantic SpeedUp','FontSize', 15); 
        ylabel('Semantic SpeedUp','FontSize', 15);
        zlabel('Objective Function','FontSize', 15);
        hold on;
        surf(Sns,Ss,OF);
        grid minor;
        plot3([SNS,SNS],[0, maxTempDist],[0, 0],'Color','r','LineWidth',2);
        plot3([0,maxTempDist],[SS, SS],[0, 0],'Color','g','LineWidth',2);
        plot3([SNS,SNS],[SS, SS],[0, Z],'Color','m','LineWidth',2);
        colormap jet;
        colorbar;
        hold off;
%         figure;        
%         title('Search Space','FontSize', 20); 
%         xlabel('Non Semantic SpeedUp','FontSize', 15); 
%         ylabel('Semantic SpeedUp','FontSize', 15);
%         zlabel('ERROR','FontSize', 15);
%         hold on;
%         surf(Sns,Ss,ERR);
%         grid minor;
%         plot3([SNS,SNS],[0, maxTempDist],[0, 0],'Color','r','LineWidth',2);
%         plot3([0,maxTempDist],[SS, SS],[0, 0],'Color','g','LineWidth',2);
%         plot3([SNS,SNS],[SS, SS],[0, Z],'Color','m','LineWidth',2);
%         colormap jet;
%         colorbar;
%         hold off;
    end    
end
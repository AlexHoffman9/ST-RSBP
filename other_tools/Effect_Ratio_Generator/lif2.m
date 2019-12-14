close all;
clearvars;
do_plot = 0;
count = 0;

%% Params
tau_m   = 64; %ms
tau_s   = 8;
t_ref   = 2;
dt      = 1;
R       = 1;

%% Simulate Spike Process
% ranges
T           = 400;
o_j         = 1:50; % j : input neuron
L_j         = length(o_j);
o_i         = 1:50; % i : output neuron
L_i         = length(o_i);
n_iter      = 100;
eij         = zeros(L_j,L_i);

for idx_j = 1:L_j
    for idx_i = 1:L_i
        % repeat results and average
        for iter = 1:n_iter
            % construct signals
            [d_j,d_i,alpha,epsilon] = deal(zeros(1,T / dt));
            
            idx_d_j = randsample(T / (t_ref / dt),o_j(idx_j));
            idx_d_i = randsample(T / (t_ref / dt),o_i(idx_i));
            
            d_j(idx_d_j * t_ref / dt) = ones(1,o_j(idx_j));
            d_i(idx_d_i * t_ref / dt) = ones(1,o_i(idx_i));
            
            % differential equations using Euler method
            for t = 1:dt:T-1
                
                alpha(t+1)      = alpha(t) + dt / tau_s * (-alpha(t) + d_j(t));
                
                % refractory period
                if count > 0
                    count           = count - dt;
                    epsilon(t+1)    = 0;
                    continue
                end
                
                epsilon(t+1)    = epsilon(t) + dt / tau_m * (-epsilon(t) + R * alpha(t));
                
                % if the neuron has fired
                if d_i(t+1) ~= 0
                    eij(idx_j,idx_i) = eij(idx_j,idx_i) + epsilon(t+1);
                    count = t_ref;
                end
            end
        end
    end
end
% average the results over the number of iterations
eij = eij / n_iter;

%% Fit polynomial
deg = 4;
pol = zeros(L_j,deg + 1);
o_i_ext = [o_i (1:10) + o_i(end)];
val = zeros(L_j,length(o_i_ext));
for idx_j = 1:L_j
    p = polyfit(o_i,eij(idx_j,:),deg);
    pol(idx_j,:) = p;
    val(idx_j,:) = polyval(p,o_i_ext);
end

figure;
mesh(o_i,o_j,eij,'edgecolor','b'); hold on;
mesh(o_i_ext,o_j,val,'edgecolor','r');
xlabel('o_i');
ylabel('o_j');
zlabel('e_{ij}');
legend('data','fit','location','northwest');

save('my_p_Tau_64_400-100.txt','pol','-ascii');

            
        
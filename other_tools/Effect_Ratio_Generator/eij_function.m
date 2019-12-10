close all;
clearvars;

%% Params
num_oi      = 1:1:50;
num_oj      = 1:1:50;
T_REF       = 2;
end_time    = 800;
TAU_M       = 64;
TAU_S       = 8;
iteration   = 1000;

%% Init
l_i = length(num_oi);
l_j = length(num_oj);
mean_firing = zeros(l_j,l_i);

%% Build Spiking Trains
for input_num_index=1:l_j
    for output_num_index=1:l_i
        fprintf('%d/%d\n',(input_num_index-1) * l_j + output_num_index, l_i*l_j);
        E_ij = zeros(1,iteration);
        for i=1:1:iteration
            input_time = random_time(num_oj(input_num_index), end_time, T_REF);
            output_time = random_time(num_oi(output_num_index), end_time, T_REF);
            E_ij(i) = eij_step(num_oj(input_num_index),num_oi(output_num_index),input_time,output_time, T_REF, TAU_M, TAU_S, end_time);
        end
        mean_firing(input_num_index,output_num_index) = mean(E_ij);
    end
end

%% Plot
figure;
mesh(num_oi,num_oj,mean_firing,'edgecolor','b','facealpha',0); xlabel('output num'); ylabel('input num'); title('means of new effect ratio \partial e_{ij}/\partial o_i');
deg = 4;
p_final = zeros(l_j,deg + 1);
mean_firing_est = zeros(size(mean_firing));
for input_num_index=1:l_j
    a = mean_firing(input_num_index,:);
    p = polyfit(num_oj,a,deg);
    p_final(input_num_index,:) = p;
    val = polyval(p,num_oj);
    mean_firing_est(input_num_index,:) = val;
end
hold on;
mesh(num_oi,num_oj,mean_firing_est,'edgecolor','r','facealpha',0);
legend('data','fit');

%% Save
out_path = append("p_Tau_", num2str(TAU_M), ' ', num2str(end_time), ".txt");
save(out_path,'p_final','-ascii');

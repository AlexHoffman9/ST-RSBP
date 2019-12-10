function output = eij_step(input_num,output_num,input_time,output_time, T_REF, TAU_M, TAU_S, end_time)
%EIJ_STEP Compute e_ij factor based on two artificial input / output
% spike trains
%   input_num / output_num : number of spikes in input / output spike
%       trains
%   input_time / output_time : input / output spike trains
%   T_REF : refractory time of a neuron after firing
%   TAU_M / TAU_S : time constants in the membrane potential / current
%       recurrence equations
%   end_time : time length of the spike trains

if input_num == 0 || output_num == 0
    output = 0;
    return;
end
output      = 0;
p           = 0;
q           = 0;
index_in    = 1;
index_out   = 1;
t_ref       = 0;

for t = 1:1:end_time
    % alpha_i(t) recurrence equation
    p = p - p / TAU_S;
    
    % new input spike
    if index_in <= input_num && input_time(index_in) == t - 1
        p = p + 1 / TAU_S;
        index_in = index_in + 1;
    end
    
    % u_i(t) recurrence equation
    q = q + (p - q) / TAU_M;
    
    % if we are in refractory mode
    if t_ref ~= 0
        q = 0;
        t_ref = t_ref - 1;
    end
    
    % new output spike
    if output_time(index_out) == t - 1
        output = output + q;
        
        index_out = index_out + 1;
        t_ref = T_REF;
        % reset potential
        q = 0;
    end
    
    % end of the spike trains
    if index_out > output_num
        break;
    end
end
end


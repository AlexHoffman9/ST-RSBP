function output = random_time(num, end_time, T_REF)
%RANDOM_TIME Constructs a spike train vector
%   num : number of spikes in the train
%   end_time : maximum time length of the spike train
%   T_REF : minimum step between two spikes
record=0;
output = [];
while(record<num)
    t=round(rand(1,1)*end_time);
    exist=0;
    for i=0:1:T_REF
        if ismember(t-i,output)||ismember(t+i,output)
            exist=1;
            break;
        end
    end
    if exist==1
        continue;
    end
    output = [output, t];
    record = record+1;
end
output=sort(output);
end


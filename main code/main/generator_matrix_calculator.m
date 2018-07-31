H = [1 1 0 1 0 0 0 0 0 1 0 0 0 1 0 0;
     0 0 1 0 1 0 1 0 0 0 0 1 0 0 1 0; 
     0 0 0 1 0 0 1 0 1 0 1 0 0 1 0 1; 
     0 1 0 0 1 0 0 1 0 0 1 0 1 0 0 0];
%      1 0 0 0 0 1 0 0 1 0 0 1 0 0 1 0;
%      0 0 1 0 0 1 0 1 0 1 0 0 1 0 0 1];
 %% generate H
 Hn = 8;
 Hm = 15;
 
 Hsparse = zeros([Hn Hm]);
 num_1s_in_column = 3
 for i =1:Hm
    n = 0;
    while (n ~=3)
     coli = sprand(Hn ,1 ,num_1s_in_column/Hn);
     n = nnz(coli);
    end
    for j = 1:Hn
        if(coli(j)~=0)
            coli(j)=1;
        end
    end
    Hsparse(:,i) = coli(:);
 end
 
 H = full(Hsparse);
% find corresponding G
[m, n] = size(H);
H
for num_rows = 1:m
    val =0;
    row_counter = num_rows-1;
    while (val==0 && row_counter<m)% identify first 1 in row
        row_counter = row_counter+1;
        val = H(row_counter, num_rows);   
    end
    
    % move to diagonal
    temp_row = H(num_rows,:);
    H(num_rows,:) = H(row_counter,:);
    H(row_counter,:) = temp_row;
    
    %sum any further rows where this column contains 1
    row_counter = 0;
    while row_counter<m % identify first 1 in row
        row_counter = row_counter+1;
        if (H(row_counter, num_rows)==1 && row_counter~=num_rows)
            H(row_counter,:) = H(row_counter,:) + H(num_rows,:);
        end
    end
    H = mod(H,2)
end


%%

 Hrref = rref(H);
 Hrref = abs(Hrref);
 Hrref = mod(Hrref, 2);
[m,n] = size(Hrref);
 Pt = Hrref(:,m+1:n);
 
 P = transpose(Pt);
 
 [m,n] = size(P);
 Ik = eye(m);
 
 G = [P Ik];
 
 
%test a code word
[m, n] = size(G)

wordsp = sprand(1, m, 0.5);


for j = 1:m
    if(wordsp(j)~=0)
        wordsp(j)=1;
    end
end

word = full(wordsp);
codeword = mod((word*G),2);
 
mod(H*transpose(codeword),2)
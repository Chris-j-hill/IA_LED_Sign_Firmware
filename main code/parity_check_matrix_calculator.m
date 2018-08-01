 Hn = 248;    %target size
 Hm = 255;
 
 Psparse = zeros(Hm-Hn,Hn);
 num_1s_in_column = 10;
 for i =1:Hm-Hn
    n = 0;
    while (n ~=num_1s_in_column)
     coli = sprand(1,Hn ,num_1s_in_column/Hn);
     n = nnz(coli);
    end
    for j = 1:Hn
        if(coli(j)~=0)
            coli(j)=1;
        end
    end
    Psparse(i,:) = coli(:);
 end
 
 P = full(Psparse);
 Ik = eye(Hm-Hn);
 G = [P Ik];
 
 Pt = transpose(P);
 Ik_n = eye(Hn);
 H = [Ik_n Pt];
 
%  mod(G*transpose(H),2)


[m, n] = size(G); 
wordsp = sprand(1, m, 0.5);

for j = 1:m
    if(wordsp(j)~=0)
        wordsp(j)=1;
    end
end

word = full(wordsp);
codeword = mod((word*G),2);
% codeword(1) = mod(codeword(1)+1,2); %test error
mod(H*transpose(codeword),2)

 
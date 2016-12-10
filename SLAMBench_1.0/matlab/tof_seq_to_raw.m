% file_name = 'Image0020.npy';
pmd_bin_seq = 'pmd_tof_500frames.dat';

% file_name_root = 'Image';


hres = 224; vres = 172;

raw_file_name = 'pmd.raw';

fid_m = fopen(raw_file_name,'wb');

rgb = zeros(vres,hres,3);
rgb = uint8(rgb);

fid_s = fopen(pmd_bin_seq,'rb');

for fr = 1:500
      
    d = fread(fid_s,38912,'uint16')';
    D = reshape(d(1:vres*hres),hres,vres)';
    
    
    D = double(D);   
    D = D * 0.3125;
    
    figure(1);
    imagesc(D/1000); axis image; axis off;
    
    
    D = D';    
    D_out = uint16(D);
    fwrite(fid_m,hres,'int32');
    fwrite(fid_m,vres,'int32');
    fwrite(fid_m,D_out(:),'uint16');
    fwrite(fid_m,hres,'int32');
    fwrite(fid_m,vres,'int32');
    fwrite(fid_m,rgb(:),'uint8');
end

fclose(fid_s);
fclose(fid_m);
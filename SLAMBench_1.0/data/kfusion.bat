mkdir output
mkdir output\config
mkdir output\pose
mkdir output\tsdf
mkdir output\vert_norm
mkdir output\render_depth
mkdir output\render_track
mkdir output\render_volume

kfusion.exe -i xtion.raw  -s 4.8 -p 0.34,0.5,0.24 -z 1 -c 1 -r 1 -v 256 -k 481.2,480,320,240
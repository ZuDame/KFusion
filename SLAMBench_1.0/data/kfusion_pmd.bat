mkdir output
mkdir output\pose
mkdir output\config
mkdir output\tsdf
mkdir output\vert_norm
mkdir output\render_depth
mkdir output\render_track
mkdir output\render_volume

kfusion.exe -i pmd.raw  -a 0.04 -s 3 -p 0.34,0.5,0.0  -z 1 -c 1 -r 1 -v 256 -k 216.907608,216.907608,111.678253,83.561287


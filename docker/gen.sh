echo $0
for dirpath in $(ls -d $(dirname $0)/*/ )
do
  echo $dirpath
  dir=$(basename $dirpath)
  echo directory $dir "{\"branch\":\"$dir\"}"
  echo "{\"branch\":\"$dir\"}" | gmcr - Dockerfile.gmcr > $dirpath/Dockerfile
done

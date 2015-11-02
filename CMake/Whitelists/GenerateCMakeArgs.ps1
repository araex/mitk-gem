param (
    [string]$whitelist = $(throw "-whitelist is required")
)

$content=Get-Content $whitelist
$pattern =  '"([^"]*)"'
$content=$content -match $pattern
$content=$content.Replace("`"","")
$cmake_args="-DMITK_BUILD_ALL_PLUGINS:BOOL=0"
Foreach($line in $content.Split("`n"))
{
    $tmp=$line.trim()
    $cmake_args="${cmake_args} -DMITK_BUILD_${tmp}:BOOL=1"
}
echo $cmake_args
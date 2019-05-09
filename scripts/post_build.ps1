. "$PSScriptRoot\helpers.ps1"

$appId = $args[0]
$libName = $args[1]
$outDir = $args[2]

$appOutDir = "$outDir\$appId"
New-Item -Path $appOutDir -ItemType Directory -ErrorAction SilentlyContinue

$dllName = "$libName.dll"
$dllOutPath = "$appOutDir\$libName.dll"
$jsonName = "$libName.json"
$jsonOutPath = "$appOutDir\$jsonName"

Move-Item -Force $outDir\$dllName $dllOutPath

Write-Host "正在转码 $jsonName 到 GB18030……"
$content = Get-Content "$projectDir\$jsonName" -Encoding UTF8 -Raw
$gb18030 = [System.Text.Encoding]::GetEncoding("GB18030")
$gb18030.GetBytes($content) | Set-Content $jsonOutPath -Encoding Byte
if ($?)
{
    Write-Success "转码 JSON 描述文件成功"
}
else
{
    Write-Failure "转码 JSON 描述文件失败"
    SafeExit 1
}

if (Test-Path "$PSScriptRoot\install.ps1")
{
    Write-Host "正在运行安装脚本 `"install.ps1 $args`"……"
    powershell.exe -ExecutionPolicy Bypass -NoProfile -File "$PSScriptRoot\install.ps1" $appId $libName $appOutDir
}

SafeExit 0

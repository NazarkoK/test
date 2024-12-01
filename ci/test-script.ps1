param (
[string]$port,
[string]$speed
)
Write-Host "`n--------------------------------------------------`n"
Write-Host "Running a script for all tests..."
Write-Host "`n--------------------------------------------------`n"
function Find-MSBuild {
$possiblePaths = @(
"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe",
"C:\Program Files (x86)\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe",
"C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe",
"C:\Program Files (x86)\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe",
"C:\Program Files\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe",
"C:\Program Files (x86)\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe"
)
foreach ($path in $possiblePaths) {
if (Test-Path $path) {
return $path
}
}
Write-Host "`n----------------------------------------------------------------------------`n"
Write-Error "MSBuild.exe not found. Please ensure it is installed and the path is correct."
Write-Host "`n----------------------------------------------------------------------------`n"
exit 1
}
$msbuildPath = Find-MSBuild
$repoDir = (Get-Item -Path $PSScriptRoot).Parent.FullName
$binDir = "$repoDir\bin"
$testResultsDir = "$repoDir\ci\test_result"
$exePathOpenCppCoverage = "$binDir\openCppCovarage-Realese\openCppCoverage.exe"
$exePathArduinoCliPath = "$binDir\arduino-cli.exe"
$hexOutputDir = "$repoDir\ci\build\server"
$sketchPath = "$repoDir\src\server\server.ino"
$outputHexPath = "$hexOutputDir\server-script-arduino-uno-r3.ino.hex"
$resultTestServerDir = "$repoDir\ci\test_result\server"
$projectTestServerPath = "$repoDir\tests\test_server"
$outputProjectTestServerDir = "$binDir\test_server"
$resutlTestServerPath = "../../test_result/server/TestResultServer.xml"
$resultTestClientDir = "$repoDir\ci\test_result\client"
$projectTestClientPath = "$repoDir\tests\test_client"
$outputProjectTestClientDir = "$binDir\test_client"
if (-not (Test-Path $exePathOpenCppCoverage)) {
Write-Host "`n--------------------------------------------------`n"
Write-Host "OpenCppCoverage not found, starting installation..."
Write-Host "`n--------------------------------------------------`n"
New-Item -ItemType Directory -Force -Path $binDir | Out-Null
Invoke-WebRequest -Uri "https://github.com/Lyhotop/openCppCovarage/archive/refs/tags/Realese.zip" -OutFile "$binDir\openCppCovarage.zip"
Expand-Archive -Path "$binDir\openCppCovarage.zip" -DestinationPath $binDir -Force
Remove-Item "$binDir\openCppCovarage.zip"
Remove-Item "$binDir\openCppCovarage-Realese\Plugins\Exporter\Tools.dll"
Write-Host "`n--------------------------------------------------`n"
Write-Host "OpenCppCoverage installation is complete!.."
Write-Host "`n--------------------------------------------------`n"
} else {
Write-Host "`n--------------------------------------------------`n"
Write-Host "OpenCppCoverage is already installed!.."
Write-Host "`n--------------------------------------------------`n"
}
if ($port -and $speed) {
if (-not (Test-Path $exePathArduinoCliPath)) {
Write-Host "`n--------------------------------------------------`n"
Write-Host "Arduino CLI not found, proceeding with installation..."
Write-Host "`n--------------------------------------------------`n"
New-Item -ItemType Directory -Force -Path $binDir | Out-Null
Invoke-WebRequest -Uri "https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Windows_64bit.zip" -OutFile "$binDir\arduino-cli.zip"
Expand-Archive -Path "$binDir\arduino-cli.zip" -DestinationPath $binDir -Force
Remove-Item "$binDir\arduino-cli.zip"
} else {
Write-Host "`n--------------------------------------------------`n"
Write-Host "Arduino CLI is already installed."
Write-Host "`n--------------------------------------------------`n"
}
Write-Host "`n--------------------------------------------------`n"
Write-Host "Configuring Arduino CLI now..."
Write-Host "`n--------------------------------------------------`n"
& $exePathArduinoCliPath config init
& $exePathArduinoCliPath core update-index
& $exePathArduinoCliPath core install arduino:avr
Write-Host "`n--------------------------------------------------`n"
Write-Host "Compiling Arduino sketch..."
Write-Host "`n--------------------------------------------------`n"
New-Item -ItemType Directory -Force -Path $hexOutputDir | Out-Null
& $exePathArduinoCliPath compile --fqbn arduino:avr:uno --output-dir $hexOutputDir $sketchPath
Write-Host "`n--------------------------------------------------`n"
Write-Host "Saving compiled HEX file..."
Write-Host "`n--------------------------------------------------`n"
Copy-Item -Path "$hexOutputDir\server.ino.hex" -Destination $outputHexPath -Force
Write-Host "`n--------------------------------------------------`n"
Write-Host "Uploading HEX file to Arduino on port $port..."
Write-Host "`n--------------------------------------------------`n"
& $exePathArduinoCliPath upload -p $port --fqbn arduino:avr:uno -i $outputHexPath
Write-Host "`n--------------------------------------------------`n"
Write-Host "Building the server testing project..."
Write-Host "`n--------------------------------------------------`n"
& $msbuildPath "$projectTestServerPath\test_server.sln" "/p:OutDir=$outputProjectTestServerDir\"
$exePathTestServer = "$outputProjectTestServerDir\test_server.exe"
$arguments = @(
"--working_dir=$projectTestServerPath\test_server",
"--source=$projectTestServerPath\test_server",
"--export_type=html:$repoDir\ci\test_result\coverage_report_server",
"--",
$exePathTestServer,
$port,
$speed,
"--gtest_output=xml:$resultTestServerDir/TestResultServer.xml"
)
& $exePathOpenCppCoverage @arguments
$xmlPath = "$resultTestServerDir\TestResultServer.xml"
$xsltPath = "$testResultsDir\convert\TestResultsToHTML.xslt"
$htmlOutputPath = "$resultTestServerDir\TestResultServer.html"
Write-Host "`n--------------------------------------------------`n"
Write-Host "Transforming test results to HTML format..."
Write-Host "`n--------------------------------------------------`n"
$xml = New-Object System.Xml.XmlDocument
$xml.Load($xmlPath)
$xslt = New-Object System.Xml.Xsl.XslCompiledTransform
$xslt.Load($xsltPath)
$xslt.Transform($xmlPath, $htmlOutputPath)
Write-Host "`n`n`n`n"
}
Write-Host "`n--------------------------------------------------`n"
Write-Host "Building the client testing project..."
Write-Host "`n--------------------------------------------------`n"
& $msbuildPath "$projectTestClientPath\test_client.sln" "/p:OutDir=$outputProjectTestClientDir\"
$exePathTestClient = "$outputProjectTestClientDir\test_client.exe"
$arguments = @(
"--working_dir=$projectTestClientPath\test_client",
"--source=$projectTestClientPath\test_client",
"--export_type=html:$repoDir\ci\test_result\coverage_report_client",
"--",
$exePathTestClient,
"--gtest_output=xml:$resultTestClientDir/TestResultClient.xml"
)
Write-Host "`n--------------------------------------------------`n"
Write-Host "Running the server test and OpenCppCoverage..."
Write-Host "`n--------------------------------------------------`n"
& $exePathOpenCppCoverage @arguments
$xmlPath = "$resultTestClientDir\TestResultClient.xml"
$xsltPath = "$testResultsDir\convert\TestResultsToHTML.xslt"
$htmlOutputPath = "$resultTestClientDir\TestResultClient.html"
Write-Host "`n--------------------------------------------------`n"
Write-Host "Transforming test results to HTML format..."
Write-Host "`n--------------------------------------------------`n"
$xml = New-Object System.Xml.XmlDocument
$xml.Load($xmlPath)
$xslt = New-Object System.Xml.Xsl.XslCompiledTransform
$xslt.Load($xsltPath)
$xslt.Transform($xmlPath, $htmlOutputPath)
Write-Host "`n`n`n--------------------------------------------------`n"
param (
    [string]$port,
    [string]$portSpeed
)

# Функція для пошуку msbuild.exe
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

    Write-Error "MSBuild.exe not found. Please ensure it is installed and the path is correct."
    exit 1
}

# Пошук MSBuild
$msbuildPath = Find-MSBuild

$repoDir = (Get-Item -Path $PSScriptRoot).Parent.FullName
$binDir = "$repoDir\bin"
$testResultsDir = "$repoDir\ci\result_test"
$hexOutputDir = "$repoDir\ci\build\"
$arduinoCliPath = "$binDir\arduino-cli.exe"
$sketchPath = "$repoDir\src\server\server.ino"
$outputHexPath = "$hexOutputDir\server-script-arduino-uno-r3.ino.hex"
$projectTestHWPath = "$repoDir\tests\test_server\test_server.sln"
$outputProjectTestHWDir = "$binDir\test_server"
$testResultPath = "../../result_test/TestResult.xml"

if (-not (Test-Path $arduinoCliPath)) {
    Write-Host "Arduino CLI not found, proceeding with installation..."
    New-Item -ItemType Directory -Force -Path $binDir | Out-Null
    Invoke-WebRequest -Uri "https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Windows_64bit.zip" -OutFile "$binDir\arduino-cli.zip"
    Expand-Archive -Path "$binDir\arduino-cli.zip" -DestinationPath $binDir -Force
    Remove-Item "$binDir\arduino-cli.zip"
} else {
    Write-Host "Arduino CLI is already installed."
}

Write-Host "Configuring Arduino CLI now..."
& $arduinoCliPath config init
& $arduinoCliPath core update-index
& $arduinoCliPath core install arduino:avr

Write-Host "Compiling Arduino sketch..."
New-Item -ItemType Directory -Force -Path $hexOutputDir | Out-Null
& $arduinoCliPath compile --fqbn arduino:avr:uno --output-dir $hexOutputDir $sketchPath

Write-Host "Saving compiled HEX file..."
Copy-Item -Path "$hexOutputDir\server.ino.hex" -Destination $outputHexPath -Force

if ($port) {
    Write-Host "Uploading HEX file to Arduino on port $port with speed $portSpeed..."
    & $arduinoCliPath upload -p $port --speed $portSpeed --fqbn arduino:avr:uno -i $outputHexPath
}

Write-Host "Building the hardware testing project..."
& $msbuildPath $projectTestHWPath "/p:OutDir=$outputProjectTestHWDir\"

if ($port) {
    $exePath = "$outputProjectTestHWDir\test_server.exe"
    $arguments = "$port $portSpeed --gtest_output=xml:--gtest_output=xml:$testResultPath"
    Write-Host "Running hardware tests..."
    Start-Process $exePath -ArgumentList $arguments -Wait

    $xmlPath = "$testResultsDir\TestResult.xml"
    $xsltPath = "$testResultsDir\TestResultsToHTML.xslt"
    $htmlOutputPath = "$testResultsDir\TestResult.html"

    Write-Host "Transforming test results to HTML format..."
    $xml = New-Object System.Xml.XmlDocument
    $xml.Load($xmlPath)
    $xslt = New-Object System.Xml.Xsl.XslCompiledTransform
    $xslt.Load($xsltPath)
    $xslt.Transform($xmlPath, $htmlOutputPath)

    Write-Host "Test results are available at: $htmlOutputPath"
} 
else {
    Write-Host "No COM port provided. Skipping upload and hardware tests."
}

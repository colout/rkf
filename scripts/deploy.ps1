$WaitTimeMs = 500
$TimeoutSeconds = 60
$TotalLoops = ($TimeoutSeconds * (1000/$WaitTimeMs))

Write-Host -NoNewline "Searching for microcontroller"
while($i -lt $TotalLoops) {
    $i++
    Write-Host -NoNewline "."

    $Drives = Get-PSDrive -PSProvider 'FileSystem'
    foreach($Drive in $drives) {
        if (Test-Path -Path ($Drive.root + "INFO_UF2.TXT")) {
            Write-Host
            Write-Host ("Microcontroller found on " + $Drive.root)

            Write-Host ("Copying " + $args[0] + " to " + $Drive.root)
            Copy-Item $args[0] -Destination $Drive.root
            Exit
        }
    }
    if ([Console]::KeyAvailable) {
        Write-Host
        Write-Host
        Write-Host "Keypress detected."
        Write-Host "Exiting without deployment."
        Exit
    }
    Start-Sleep -Milliseconds $WaitTimeMs
}

Write-Host
Write-Host
Write-Host "Timed out."
Write-Host "Exiting without deployment."
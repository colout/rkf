
Write-Host -NoNewline "Searching for microcontroller"
while($i -lt 60) {
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
    Start-Sleep -Seconds 1
}

Import("env")
env.Replace(PROGNAME="ESP_Relay_v%s" % env.GetProjectOption("release_version"))
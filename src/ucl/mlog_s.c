    if (_loggers[logNo] == NULL)
        mlogInit_(logNo, NULL, NULL, M_DEBUG);

    if (_loggers[logNo]->level > level)

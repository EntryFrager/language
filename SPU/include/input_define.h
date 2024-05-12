DEF_CMD (HLT, false, 
    {
        data->cmd[pos_cmd].command = command;   \
        break;
    })

DEF_CMD (OUT, false,
    {
        data->cmd[pos_cmd].command = command;   \
        break;
    })

DEF_CMD (PUSH + (1 << 4), true, 
    {
        data->cmd[pos_cmd].command = command;
        data->cmd[pos_cmd].argc = data->buf[++id];
        
        break;
    })

DEF_CMD (PUSH + (1 << 5), true, 
    {
        data->cmd[pos_cmd].command = command;
        data->cmd[pos_cmd].reg = data->buf[++id];
        
        break;
    })

DEF_CMD (POP + (1 << 5), true,
    {
        data->cmd[pos_cmd].command = POP + (1 << 5);
        data->cmd[pos_cmd].reg = data->buf[++id];
        
        break;
    })

DEF_CMD (ADD, false, 
    {
        data->cmd[pos_cmd].command = command;   \
        break;
    })

DEF_CMD (SUB, false, 
    {
        data->cmd[pos_cmd].command = command;   \
        break;
    })

DEF_CMD (MUL, false, 
    {
        data->cmd[pos_cmd].command = command;   \
        break;
    })

DEF_CMD (DIV, false, 
    {
        data->cmd[pos_cmd].command = command;   \
        break;
    })

DEF_CMD (SIN, false, 
    {
        data->cmd[pos_cmd].command = command;   \
        break;
    })

DEF_CMD (COS, false, 
    {
        data->cmd[pos_cmd].command = command;   \
        break;
    })

DEF_CMD (SQRT, false, 
    {
        data->cmd[pos_cmd].command = command;   \
        break;
    })

DEF_CMD (IN, false, 
    {
        data->cmd[pos_cmd].command = command;   \
        break;
    })

DEF_CMD (POP, false, 
    {
        data->cmd[pos_cmd].command = command;   \
        break;
    })
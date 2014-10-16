package com.easydlna.application;

import com.easydlna.dlna.service.IDMSService;

import android.os.RemoteException;

public abstract class Command {
    public final static int CMD_TYPE_UNKNOWN = -1;
    public final static int CMD_TYPE_SETNAME = 0;
    public final static int CMD_TYPE_GETNAME = 1;
    public final static int CMD_TYPE_SETCONTENTS = 2;
    public final static int CMD_TYPE_SETAUTOSTART = 3;
    public final static int CMD_TYPE_GETAUTOSTART = 4;
    public final static int CMD_TYPE_START_STOP_DMS = 5;

    public interface ICommandCompleted {
        boolean commandCompleted(Command command, int result);
    }

    public int cmdType = CMD_TYPE_UNKNOWN;
    protected ICommandCompleted iCmdCompleted = null;
    protected Object tagObject = null;
    protected int cmdResult = -1;

    public Command(ICommandCompleted iCommandCompleted, Object argObject) {
        this.iCmdCompleted = iCommandCompleted;
        tagObject = argObject;
    }

    public int getResult() {
        return cmdResult;
    };

    public int execute() {
        if (tagObject == null) {
            if (iCmdCompleted != null) {
                iCmdCompleted.commandCompleted(this, -1);
            }
            return 0;
        }
        internalExecute();
        callComplete();
        return 0;
    }

    abstract protected int internalExecute();

    private void callComplete() {
        if (iCmdCompleted != null) {
            iCmdCompleted.commandCompleted(this, this.getResult());
        }
    }
}

class CMDSetName extends Command {
    public String newNameString;
    public boolean bImmediatly;

    public CMDSetName(String newName, boolean bImm,
            ICommandCompleted iCommandCompleted, Object argObject) {
        super(iCommandCompleted, argObject);
        newNameString = newName;
        bImmediatly = bImm;
        cmdType = CMD_TYPE_SETNAME;
    }

    @Override
    protected int internalExecute() {
        IDMSService dms = (IDMSService) tagObject;
        try {
            dms.setServerFriendlyName(newNameString, bImmediatly);
            cmdResult = 0;
        } catch (RemoteException e) {
            e.printStackTrace();
            cmdResult = -1;
        }
        return 0;
    }
}

class CMDSetContents extends Command {
    public int sharedType = -1;

    public CMDSetContents(int nSharedType, ICommandCompleted iCommandCompleted,
            Object argObject) {
        super(iCommandCompleted, argObject);
        cmdType = CMD_TYPE_SETCONTENTS;
        sharedType = nSharedType;
    }

    @Override
    protected int internalExecute() {
        IDMSService dms = (IDMSService) tagObject;
        try {
            dms.setSharedType(sharedType);
            cmdResult = 0;
        } catch (RemoteException e) {
            e.printStackTrace();
            cmdResult = -1;
        }
        return 0;
    }
}

class CMDSetAutoStart extends Command {
    public boolean bAutoStart;

    public CMDSetAutoStart(Boolean bASutoStart,
            ICommandCompleted iCommandCompleted, Object argObject) {
        super(iCommandCompleted, argObject);
        cmdType = CMD_TYPE_SETAUTOSTART;
        this.bAutoStart = bASutoStart;
    }

    @Override
    protected int internalExecute() {
        IDMSService dms = (IDMSService) tagObject;
        try {
            dms.setAutoStartup(bAutoStart);
            cmdResult = 0;
        } catch (RemoteException e) {
            e.printStackTrace();
            cmdResult = -1;
        }

        return 0;
    }
}

class CMDTriggerDms extends Command {

    public boolean bStarted = false;

    public CMDTriggerDms(boolean start, ICommandCompleted iCommandCompleted,
            Object argObject) {
        super(iCommandCompleted, argObject);
        cmdType = CMD_TYPE_START_STOP_DMS;
        bStarted = start;
    }

    @Override
    protected int internalExecute() {
        IDMSService dms = (IDMSService) tagObject;
        try {
            cmdResult = dms.start_stop_DMS(bStarted);
        } catch (RemoteException e) {
            e.printStackTrace();
            cmdResult = -1;
        }
        return 0;
    }
}

class CMDBroadcast extends Command {
    public int timeOut = 1800;

    public CMDBroadcast(int timeOut, ICommandCompleted iCommandCompleted,
            Object argObject) {
        super(iCommandCompleted, argObject);
        this.timeOut = timeOut;
    }

    @Override
    protected int internalExecute() {
        IDMSService dms = (IDMSService) tagObject;
        try {
            dms.sendBroadcast(timeOut);
            cmdResult = 0;
        } catch (RemoteException e) {
            e.printStackTrace();
            cmdResult = -1;
        }
        return 0;
    }
}
